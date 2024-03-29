#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(zuk, 0.0.1, [kamasamikon@gmail.com])
AM_INIT_AUTOMAKE(zuk, 0.0.1)
AC_CONFIG_SRCDIR([src/main/zuk_fw.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h fcntl.h langinfo.h locale.h memory.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/vfs.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([clock_gettime getcwd gethostbyname memset mkdir nl_langinfo rmdir setlocale socket strchr strerror strrchr strstr strtol strtoul])

#debug options support
AC_ARG_ENABLE([debug],
    [AS_HELP_STRING([--enable-debug],[debug program(default is no)])],
    [CFLAGS="${CFLAGS} -DDEBUG -g"],
    [CFLAGS="-O2"])


AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/lib/Makefile
                 src/lib/kacoo/Makefile
                 src/lib/kmc/Makefile
                 src/lib/kmm/Makefile
                 src/lib/textconv/Makefile
                 src/main/Makefile
                 src/mod/Makefile
                 src/mod/local/Makefile
                 src/mod/playlist/Makefile
                 src/mod/ui/Makefile])
AC_OUTPUT
