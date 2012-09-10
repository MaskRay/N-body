CXX = @CXX@

bin_PROGRAMS = n-body
n_body_SOURCES = Cmdline.c Config.cc Main.cc Cmdline.h Collide.hh ProcessScheduler.hh XFormat.hh BarnesHut.cc Core.cc ProcessScheduler.cc ThreadScheduler.cc BarnesHut.hh Config.hh Format.cc Format.hh QuadTree.hh Collide.cc IntervalTree.cc QuadTree.cc XFormat.cc Core.hh IntervalTree.hh ThreadScheduler.hh BSPTree.hh BSPTree.cc SegmentTree.hh SegmentTree.cc OctTree.hh OctTree.cc
n_body_CPPFLAGS = $(OPENMP_CXXFLAGS) $(MPI_CXXFLAGS) $(X11_CFLAGS) $(x11_CFLAGS)
n_body_LDFLAGS = -pthread $(OPENMP_CXXFLAGS) $(MPI_CXXLDFLAGS) $(X11_LIBS)
EXTRA_DIST = Cmdline.ggo

Cmdline.c Cmdline.h: Cmdline.ggo
	gengetopt --file-name Cmdline < $<
