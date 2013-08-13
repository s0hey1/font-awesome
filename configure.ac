AC_INIT([font-awesome], [1.4.0], [josh@creativemarket.com], [font-awesome], [https://www.creativemarket.com])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])
AC_PROG_CXX
AC_PROG_LIBTOOL
LT_INIT([disable-static])

AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

BOOST_REQUIRE([1.40])
BOOST_TEST
BOOST_PROGRAM_OPTIONS

# use pkg-config
PKG_PROG_PKG_CONFIG([0.2])

PKG_CHECK_MODULES([libpng], [libpng])
PKG_CHECK_MODULES([freetype2], [freetype2])

AC_ARG_WITH(jpeg, 
	[AC_HELP_STRING([--with-jpeg=<DIR>],
    [Provide an alternative path to the libJPEG library directory])],
    libjpeg_libdir=$withval,
    libjpeg_libdir=""
    )

# Check for -ljpeg
AC_CHECK_LIB(jpeg, jpeg_destroy_decompress,
                   jpeg_ok=yes, jpeg_ok=no
                   AC_MSG_ERROR([JPEG library (libjpeg) not found]))
if test "$jpeg_ok" = yes; then
   AC_MSG_CHECKING([for jpeglib])
   AC_TRY_CPP(
[#include <stdio.h>
#undef PACKAGE
#undef VERSION
#undef HAVE_STDLIB_H
#include <jpeglib.h>],
         jpeg_ok=yes,
         jpeg_ok=no)
   AC_MSG_RESULT($jpeg_ok)
   if test "$jpeg_ok" = yes; then
      JPEG_LIBS="-ljpeg"
      # should we check for progressive JPEG like GTK+ as well?
   else
      AC_MSG_ERROR([JPEG library (libjpeg) not found])
   fi
fi
AC_SUBST([JPEG_LIBS])


# gcov support (for code coverage reports)

AC_PATH_PROG([GCOV], [gcov], [enable_gcov=no])

AC_MSG_CHECKING([whether to build with gcov testing])

AC_ARG_ENABLE([gcov],
              [AS_HELP_STRING([--enable-gcov],
                              [Whether to enable coverage testing (requires gcc and gcov)])],
              [],
              [enable_gcov=no])

AS_IF([test "x$enable_gcov" = "xyes" && test "x$GCC" = "xyes"],
      [
        GCOV_CFLAGS="-g -O0 -fprofile-arcs -ftest-coverage"
        GCOV_LDFLAGS="-lgcov"
      ]
)

AM_CONDITIONAL([GCOV_ENABLED], [test "x$enable_gcov" = "xyes"])
AC_SUBST([GCOV_CFLAGS])
AC_SUBST([GCOV_LDFLAGS])
AC_MSG_RESULT([$enable_gcov])

AC_CONFIG_FILES([
Makefile
version.h
src/Makefile
tests/Makefile
])

AC_OUTPUT
