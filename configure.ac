# Define the package version numbers, bug reporting link, project homepage.
m4_define([SQIDS_VERSION_STRING], [0.2.1])
m4_define([SQIDS_BUGREPORT_URL], [https://github.com/sqids/sqids-c/issues])
m4_define([SQIDS_PROJECT_URL], [https://github.com/sqids/sqids-c])

# Initialize autoconf & automake.
AC_PREREQ([2.71])
AC_INIT([sqids], [SQIDS_VERSION_STRING], [SQIDS_BUGREPORT_URL], [sqids], [SQIDS_PROJECT_URL])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.9 foreign])

# Checks for programs.
AC_PROG_CC()
AC_PROG_INSTALL()
AC_PROG_MAKE_SET()

# Libtool.
LT_INIT()

# GLibC's math.
AC_CHECK_LIB([m], [ceil], [LIBM="-lm"])
AC_SUBST([LIBM])

# Thread-local storage.
AX_TLS([:], [:])

# Find pkg-config.
PKG_PROG_PKG_CONFIG()
PKG_NOARCH_INSTALLDIR()

# Default blocklist.
AC_ARG_ENABLE([default-blocklist], AS_HELP_STRING([--enable-default-blocklist], [Enable default blocklist @<:@default=yes@:>@]), [
  case "${enableval}" in
    yes) SQIDS_DEFAULT_BLOCKLIST="1";;
    no)  SQIDS_DEFAULT_BLOCKLIST="0";;
    *)   AC_MSG_ERROR(["bad value ${enableval} for feature --enable-default-blocklist"]);;
  esac
], [
  SQIDS_DEFAULT_BLOCKLIST="1"
])
AC_DEFINE_UNQUOTED([SQIDS_DEFAULT_BLOCKLIST], [${SQIDS_DEFAULT_BLOCKLIST}], [Build the library with the default blocklist included.])

# Debug.
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [Enable debugging @<:@default=no@:>@.]), [
  case "${enableval}" in
    yes) SQIDS_DEBUG="1"; CFLAGS="${CFLAGS} -g -fsanitize=address -fno-omit-frame-pointer";;
    no)  SQIDS_DEBUG="0";;
    *)   AC_MSG_ERROR(["bad value ${enableval} for feature --enable-debug"]);;
  esac
], [
  SQIDS_DEBUG="0"
])
AM_CONDITIONAL([SQIDS_DEBUG], [test "x${SQIDS_DEBUG}" = "x1"])
AC_DEFINE_UNQUOTED([SQIDS_DEBUG], [${SQIDS_DEBUG}], [Debug mode.])

# Makefiles.
AC_CONFIG_FILES([Makefile src/Makefile sqids.pc])

# Le grand finale.
AC_OUTPUT()
