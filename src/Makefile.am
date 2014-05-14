include_HEADERS = pwtypes.h pwinterface.h pw_IPaint.h pw_IRead.h pw_IWrite.h \
	pwutil.h pwtilemap.h
lib_LTLIBRARIES = libpwutil.la libpwtilemap.la

AM_CFLAGS = -Wall

PWUTIL_VERSION=6:0:5
libpwutil_la_SOURCES = pwutil.c pwdefs.c pwglog.c pwthrottle.c pwnull.c
libpwutil_la_CPPFLAGS = $(PW_GLIB_CFLAGS)
libpwutil_la_LDFLAGS = -version-info $(PWUTIL_VERSION)
libpwutil_la_LIBADD = $(PW_GLIB_LIBS) -lrt

PWTILEMAP_VERSION=4:0:3
libpwtilemap_la_SOURCES = pwtilemap.c
libpwtilemap_la_CPPFLAGS = $(PW_GLIB_CFLAGS)
libpwtilemap_la_LDFLAGS = -version-info $(PWTILEMAP_VERSION)
libpwtilemap_la_LIBADD = -lpwutil $(PW_GLIB_LIBS)
