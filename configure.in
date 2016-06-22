#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.63)
AC_INIT([convreverse], [0.1], [sydro@salug.it])
AM_INIT_AUTOMAKE([1.10])
AC_CONFIG_SRCDIR([src/convreverse.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_MSG_CHECKING([Checking for pidgin])
if pkg-config --exists pidgin ; then
   AC_MSG_RESULT(yes)
else
   AC_MSG_RESULT(no)
   exit
fi

par_includes=[`pkg-config --cflags pidgin`]
par_ldflags=[`pkg-config --libs pidgin`]
AC_SUBST(par_includes)
AC_SUBST(par_ldflags)

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
