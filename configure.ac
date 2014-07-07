AC_INIT([libnoobcsv], [0.0.0])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([src/noobcsv.c])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC
AC_PROG_CC_STDC([c11])
AM_PROG_AR

AC_CONFIG_HEADERS([config.h])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

LT_INIT

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])

AC_OUTPUT
