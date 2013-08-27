AC_INIT([font-awesome], [1.4.0], [josh@creativemarket.com], [font-awesome], [https://www.creativemarket.com])
AC_PREREQ([2.59])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.10 -Wall no-define])

AX_CHECK_ENABLE_DEBUG([no])
AC_PROG_CXX
AC_PROG_LIBTOOL
AX_COMPILER_VENDOR
LT_INIT([disable-static])

AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

if test "x$ax_enable_debug" != "xyes"; then
  AX_APPEND_COMPILE_FLAGS([-O3])
fi

AC_LANG_PUSH([C++])
AX_CXX_CPPFLAGS_STD_LANG(AM_CPPFLAGS)
AX_CXX_CXXFLAGS_STD_LANG(AM_CXXFLAGS)
AX_CXX_LDFLAGS_STD_LANG(AM_LDFLAGS)
AX_CXXFLAGS_WARN_ALL
AC_LANG_POP([C++])
AX_WARNINGS_SANITIZE
AC_CACHE_SAVE

#AX_CHECK_ICU([4.2])
m4_include(m4/icu.m4)

BOOST_REQUIRE([1.40])
BOOST_PROGRAM_OPTIONS

if test "x$ax_enable_debug" == "xyes"; then
  BOOST_TEST
fi

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
        GCOV_CFLAGS="-fprofile-arcs -ftest-coverage"
        GCOV_LDFLAGS="-lgcov"
      ]
)

AM_CONDITIONAL([GCOV_ENABLED], [test "x$enable_gcov" = "xyes"])
AM_CONDITIONAL([TESTS_ENABLED], [test "x$ax_enable_debug" = "xyes"])
AC_SUBST([GCOV_CFLAGS])
AC_SUBST([GCOV_LDFLAGS])
AC_MSG_RESULT([$enable_gcov])

AC_CONFIG_FILES([
Makefile
version.h
src/Makefile
])

if test "x$ax_enable_debug" = "xyes"; then
  AC_CONFIG_FILES([tests/Makefile])
fi


AC_OUTPUT
