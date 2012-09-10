#include "config.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

#include <stdlib.h>
#include <algorithm>
#include "ProcessScheduler.hh"
#include "ThreadScheduler.hh"
#include "Format.hh"
#include "XFormat.hh"
#include "Config.hh"

Master::Master(const Task& tk_) : tk(tk_)
{
  tk.bodies = cfg.load();

#ifdef USE_MPI
  int chunk_size = (tk.n + cfg.nprocs - 1) / cfg.nprocs;
  tk.bgn = 0;
  tk.end = chunk_size;
  for (int i = 1; i < cfg.nprocs; i++) {
    tk.bgn = std::min(tk.bgn + chunk_size, tk.n);
    tk.end = std::min(tk.end + chunk_size, tk.n);
    MPI_Send(&tk, sizeof tk, MPI_BYTE, i, INITIAL_TASK, MPI_COMM_WORLD);
  }
  tk.bgn = 0;
  tk.end = chunk_size;
#else
  tk.bgn = 0;
  tk.end = tk.n;
#endif
}

void Master::advance(ft dt)
{
  if (cfg.nprocs == 1) {
    tk.dt = dt;
    threadSchedule(tk);
  } else {
#ifdef USE_MPI
    int msg = ADVANCE;
#ifdef USE_FLOAT
    MPI_Bcast(&dt, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
#else
    MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#endif
    MPI_Bcast(tk.bodies, sizeof(Body) * tk.n, MPI_BYTE, 0, MPI_COMM_WORLD);
    tk.dt = dt;
    threadSchedule(tk);
    MPI_Gather(tk.bodies, sizeof(Body) * (tk.end - tk.bgn), MPI_BYTE, tk.bodies, sizeof(Body) * (tk.end - tk.bgn), MPI_BYTE, 0, MPI_COMM_WORLD);
#endif
  }

  int i = 0;

  nBodyCollide(tk);
  nBodyMove(tk);
}

Master::~Master()
{
  delete[] tk.bodies;
#ifdef USE_MPI
  ft msg = -1;
# ifdef USE_FLOAT
  MPI_Bcast(&msg, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
# else
  MPI_Bcast(&msg, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
# endif
#endif
}

#ifdef USE_MPI
static void worker(Task tk, int myid)
{
  ft dt;
  MPI_Request request;
  MPI_Recv(&tk, sizeof tk, MPI_BYTE, 0, INITIAL_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  Body* bodies = new Body[tk.n];
  tk.bodies = bodies;
  int i=0;
  for(;;) {
#ifdef USE_FLOAT
    MPI_Bcast(&dt, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
#else
    MPI_Bcast(&dt, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#endif
    if (dt < 0)
      return;
    MPI_Bcast(bodies, sizeof(Body) * tk.n, MPI_BYTE, 0, MPI_COMM_WORLD);
    tk.dt = dt;
    threadSchedule(tk);
    MPI_Gather(bodies + tk.bgn, sizeof(Body) * (tk.end - tk.bgn), MPI_BYTE, NULL, 0, MPI_BYTE, 0, MPI_COMM_WORLD);
  }
}
#endif

void processSchedule()
{
  int myid = 0;
  Format* output = NULL;
  Task tk = cfg.task();
#ifdef USE_MPI
  MPI_Comm_size(MPI_COMM_WORLD, &cfg.nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  if (myid == 0) {
#endif
    switch (cfg.format) {
    case FORMAT_X:
      output = new XFormat(tk);
      break;
    case FORMAT_TEXT:
      output = new TextFormat(tk);
      break;
    case FORMAT_TIME:
      output = new TimeFormat(tk);
      break;
    default:
      exit(1);
    }
#ifdef USE_MPI
  }
  int i=0;
  if (myid != 0)
    worker(tk, myid);
  else {
#endif
    output->render();
    delete output;
#ifdef USE_MPI
  }
#endif
}
