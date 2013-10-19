AC_PREREQ(2.53)
AC_INIT([conserve], [0.2.0])

AM_INIT_AUTOMAKE
AC_PROG_CXX

AC_DEFINE(DEBUG)

AX_CXX_COMPILE_STDCXX_11(,mandatory)

# pthreads aren't directly used by Conserve but seem to be linked by protobuf.
AC_CHECK_LIB(pthread, pthread_join)

AC_CONFIG_FILES([
  Makefile
  ])
AC_OUTPUT
