#include "config.h"

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif

#include "ThreadScheduler.hh"
#include "BarnesHut.hh"
#include "Config.hh"

static int numThreads()
{
#if defined(HAVE_SCHED_H) && (defined(USE_OPENMP) || defined(USE_PTHREAD))
   cpu_set_t cs;
   CPU_ZERO(&cs);
   sched_getaffinity(0, sizeof(cs), &cs);
   int cnt = 0;
   for (int i = 0; i < CPU_SETSIZE; ++i)
      cnt += CPU_ISSET(i, &cs);
   return cnt;
#else
   return 1;
#endif
}

static void* runWorker(void* data)
{
  nBodyAccelerate(*(Task*)data);
  delete (Task*)data;
  return NULL;
}

void threadSchedule(Task tk)
{
  if (cfg.nthreads <= 0)
    cfg.nthreads = numThreads(); // detect
  if (cfg.nthreads == 1) {
    if (cfg.barnesHut)
      barnesHutNew(tk);
    nBodyAccelerate(tk);
    if (cfg.barnesHut)
      barnesHutDestroy();
    return;
  }

  if (cfg.barnesHut)
    barnesHutNew(tk);
#ifdef USE_PTHREAD
  pthread_t* workers = new pthread_t[cfg.nthreads - 1];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  int chunk_size = (tk.end - tk.bgn + cfg.nthreads - 1) / cfg.nthreads;
  Task tk2 = tk;
  tk2.bgn = tk.bgn;
  tk2.end = tk.bgn + chunk_size;
  for (int i = 0; i < cfg.nthreads - 1; i++) {
    tk2.bgn = std::min(tk2.bgn + chunk_size, tk.end);
    tk2.end = std::min(tk2.end + chunk_size, tk.end);
    pthread_create(&workers[i], NULL, runWorker, new Task(tk2));
  }
  tk2.bgn = tk.bgn;
  tk2.end = tk.bgn + chunk_size;
  nBodyAccelerate(tk2);
  for (int i = 0; i < cfg.nthreads - 1; i++)
    pthread_join(workers[i], NULL);
  pthread_attr_destroy(&attr);
  delete[] workers;
#else
# ifdef USE_OPENMP
  nBodyAccelerate(tk);
# else
  fprintf(stderr, "please enable openmp/pthread to run with multiple threads\n");
  exit(3);
# endif
#endif
  if (cfg.barnesHut)
    barnesHutDestroy();
}
