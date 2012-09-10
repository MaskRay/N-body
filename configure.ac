AC_INIT([n-body], [0.1], [emacs.ray@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG([C++])
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile report/Makefile])

PKG_CHECK_MODULES([X11], [x11])

AC_ARG_WITH([pthread], AC_HELP_STRING([--with-pthread], [Enable pthread support]))
AC_ARG_WITH([mpi], AC_HELP_STRING([--with-mpi], [Enable mpi support]))
AC_ARG_WITH([openmp], AC_HELP_STRING([--with-openmp], [Enable openmp support]))
AC_ARG_WITH([float], AC_HELP_STRING([--with-float], [Use float]))

m4_include([m4/lx_find_mpi.m4])
AC_CHECK_HEADERS([sched.h])

AS_CASE(["$with_float"], [yes], [AC_DEFINE([USE_FLOAT], [], [Use float])])
AS_CASE(["$with_pthread"], [yes], [AC_DEFINE([USE_PTHREAD], [], [Use pthread])])
AS_CASE(["$with_mpi"], [yes], [
         LX_FIND_MPI AC_DEFINE([USE_MPI], [], [Use mpi])
         CXX="$MPICXX"
         ])
AS_CASE(["$with_openmp"], [yes], [AC_OPENMP AC_DEFINE([USE_OPENMP], [], [Use openmp])])

AC_OUTPUT
