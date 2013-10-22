# Prelude
AC_INIT([pwlibs], [1.6], [bug-report@piwall.co.uk])
LT_INIT
#  Sanity check
AC_CONFIG_SRCDIR([src/pwutil.c])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
# Checks for programs
AC_PROG_CC
#AM_PROG_CC_C_O
# Checks for libraries
PKG_CHECK_MODULES([PW_GLIB], [glib-2.0])
# Checks for header files
# Checks for typedefs, structures and compiler characteristics
# Checks for library functions
# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
