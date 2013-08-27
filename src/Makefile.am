AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS} -I .

bin_PROGRAMS = font-awesome

font_awesome_SOURCES = \
main.cxx \
Color.cxx \
FilePointer.cxx \
Font.cxx \
FontAwesomeException.cxx \
Image.cxx \
ImageWriter.cxx \
PNGWriter.cxx \
Printer.cxx \
Renderer.cxx

AM_CPPFLAGS = $(DEPS_CFLAGS) $(GCOV_CFLAGS) $(libpng_CFLAGS) $(freetype2_CFLAGS) $(BOOST_CPPFLAGS) $(ICU_CPPFLAGS)
AM_LDFLAGS = $(GCONV_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS) $(GCOV_LDFLAGS)

font_awesome_LDADD = $(DEPS_LIBS) \
$(libpng_LIBS) \
$(freetype2_LIBS) \
$(BOOST_PROGRAM_OPTIONS_LIBS) \
$(ICU_LIBS)
