#include "config.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "Config.hh"
#include "ProcessScheduler.hh"

int main(int argc, char* argv[])
{
#ifdef USE_MPI
  MPI_Init(&argc, &argv);
#endif

  int ret = cfg.getopt(argc, argv);
  if (ret)
    return ret;
  processSchedule();

#ifdef USE_MPI
  MPI_Finalize();
#endif
}
