include_HEADERS = pwtypes.h pwutil.h pwtilemap.h
lib_LTLIBRARIES = libpwutil.la libpwtilemap.la

PWUTIL_VERSION=1:0:0
libpwutil_la_SOURCES = pwutil.c
libpwutil_la_CPPFLAGS = $(PW_GLIB_CFLAGS)
libpwutil_la_LDFLAGS = -version-info $(PWUTIL_VERSION)
libpwutil_la_LIBADD = $(PW_GLIB_LIBS)

PWTILEMAP_VERSION=1:0:0
libpwtilemap_la_SOURCES = pwtilemap.c
libpwtilemap_la_CPPFLAGS = $(PW_GLIB_CFLAGS)
libpwtilemap_la_LDFLAGS = -version-info $(PWTILEMAP_VERSION)
libpwtilemap_la_LIBADD = -lpwutil $(PW_GLIB_LIBS)
