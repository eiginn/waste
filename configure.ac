#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT(WASTE, 1.5, , waste)
AC_CONFIG_SRCDIR([main.cpp])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE(1.8)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_OPTIONS_WXCONFIG

# Checks for libraries.
AM_PATH_WXCONFIG(2.6.0,,[AC_MSG_ERROR(wxWidgets >= 2.6.0 is required)])
CFLAGS="$CFLAGS $WX_CFLAGS -D_DEFINE_WXUI"
CXXFLAGS="$CXXFLAGS $WX_CXXFLAGS -D_DEFINE_WXUI"

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPE(__int64, int64_t)
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([gethostbyname gettimeofday inet_ntoa memset mkdir select socket strdup strrchr strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
