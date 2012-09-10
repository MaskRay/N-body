#ifndef CONFIG_HH
#define CONFIG_HH

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include "Core.hh"
#include "Cmdline.h"

enum FormatType
{
  FORMAT_X, FORMAT_TEXT, FORMAT_TIME
};

class Config
{
public:
  Config() : nprocs(1) {}
  Task task() {
    Task tk;
    tk.n = n;
    return tk;
  }
  ~Config() { cmdline_parser_free(&args_info); }
  int getopt(int &argc, char* argv[]) {
    if (cmdline_parser(argc, argv, &args_info) != 0)
      return 1;
    width = args_info.width_arg;
    height = args_info.height_arg;
    length = args_info.length_arg;
    n = args_info.n_arg;
    nthreads = args_info.nthreads_arg;
    gravitational = args_info.gravitational_arg;
    barnesHut = args_info.barnes_hut_arg != 0;
    collision_detection = args_info.collision_detection_arg;
    iter = args_info.iter_arg;
    fps = args_info.fps_arg;
    dimension = args_info.dimension_arg;

    if (! strcmp(args_info.format_arg, "x"))
      format = FORMAT_X;
    else if (! strcmp(args_info.format_arg, "text"))
      format = FORMAT_TEXT;
    else if (! strcmp(args_info.format_arg, "time"))
      format = FORMAT_TIME;
    else
      format = (FormatType)-1;

    switch (dimension) {
    case 2:
    case 3:
      break;
    default:
      fprintf(stderr, "-d ? (2 <= ? <= 3)\n");
      exit(1);
    }

    if (dimension == 3)
      switch (collision_detection) {
      case 0: break;
      case 1: case 3:
              fprintf(stderr, "Sweep line algorithm can only be used in 2-dimensioncal case\n");
              exit(1);
      case 2: break;
      default:
              fprintf(stderr, "-c ? (0 <= ? < 4)\n");
              exit(1);
      }

    return 0;
  }
  Body* load() {
    if (args_info.random_arg)
      return makeBodies();
    Body* bodies = new Body[n];
    for (int i = 0; i < n; i++) {
      Body& b = bodies[i];
#ifdef USE_FLOAT
#define F "%f"
#else
#define F "%lf"
#endif
      if (scanf(F F F F F F F F, &b.pos.x, &b.pos.y, &b.pos.z, &b.v.x, &b.v.y, &b.v.z, &b.mass, &b.r) != 8 || b.pos.x < 0 || b.pos.x > width || b.pos.y < 0 || b.pos.y > height || b.pos.z < 0 || b.pos.z > length)
        exit(2);
    }
    return bodies;
  }
  int n; // number of bodies
  ft width, height, length;
  bool barnesHut; // enable Barnes-Hut simulation
  int collision_detection; // enable fast collision detection
  int dimension;
  bool random;
  int iter; // iteration count
  int fps; // frames per second
  int nthreads, nprocs;
  ft gravitational; // gravitational constant
  FormatType format; // file containing initial state
protected:
  gengetopt_args_info args_info;
};

extern Config cfg;

#endif /* end of include guard: CONFIG_HH */
