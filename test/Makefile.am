TESTS = check_noobcsv
check_PROGRAMS = check_noobcsv
check_noobcsv_SOURCES = check_noobcsv.c noobcsv.h
check_noobcsv_CFLAGS = @CHECK_CFLAGS@
check_noobcsv_CPPFLAGS = -I $(top_builddir)/src
check_noobcsv_LDADD = $(top_builddir)/src/libnoobcsv.la @CHECK_LIBS@
