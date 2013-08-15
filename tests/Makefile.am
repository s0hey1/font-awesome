AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I .

bin_PROGRAMS = unit_tests
unit_tests_SOURCES = \
TestMain.cxx \
../src/Color.cxx \
../src/Image.cxx \
ColorTest.cxx \
ImageTest.cxx

AM_CPPFLAGS = $(DEPS_CFLAGS) $(BOOST_CPPFLAGS) $(GCOV_CFLAGS)
AM_LDFLAGS = $(BOOST_UNIT_TEST_FRAMEWORK_LDFLAGS) $(GCOV_LDFLAGS)


unit_tests_LDADD = $(DEPS_LIBS) \
$(BOOST_UNIT_TEST_FRAMEWORK_LIBS) \
$(libpng_LIBS) $(JPEG_LIBS) $(freetype2_LIBS)