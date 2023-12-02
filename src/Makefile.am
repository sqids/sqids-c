#
# Headers to install.
#

include_HEADERS = sqids.h


#
# Libraries to build.
#

lib_LTLIBRARIES = libsqids.la

libsqids_la_SOURCES = sqids.c bl.c
libsqids_la_LIBADD = $(LIBM)
libsqids_la_LDFLAGS = -no-undefined -version-number 1:0:0


#
# Binaries to build & install.
#

bin_PROGRAMS = sqids

sqids_SOURCES = main.c
sqids_LDADD = $(LIBM) libsqids.la


#
# Binaries to build & keep.
#

noinst_PROGRAMS = test_bl test_shuffle test_sqids

test_bl_SOURCES = test_bl.c
test_bl_LDADD = $(LIBM) libsqids.la

test_shuffle_SOURCES = test_shuffle.c
test_shuffle_LDADD = $(LIBM) libsqids.la

test_sqids_SOURCES = test_sqids.c
test_sqids_LDADD = $(LIBM) libsqids.la


#
# Tests.
#

TESTS=test_bl test_shuffle test_sqids
