lib_LTLIBRARIES = libpwutil.la
PWUTIL_VERSION=1:0:0
libpwutil_la_SOURCES = pwutil.c
libpwutil_la_CPPFLAGS = $(PW_GLIB_CFLAGS)
libpwutil_la_LDFLAGS = -version-info $(PWUTIL_VERSION)
libpwutil_la_LIBADD = $(PW_GLIB_LIBS)
include_HEADERS = pwtypes.h pwutil.h
