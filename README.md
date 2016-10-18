# Overview

font-awesome is a command line tool to render TTF/OTF fonts to PNG.

# Dependencies

* [FreeType 2](http://www.freetype.org/freetype2/)
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* [boost](http://www.boost.org/)
* [ICU](http://site.icu-project.org/)
* [HarfBuzz](http://www.freedesktop.org/wiki/Software/HarfBuzz/)
* [Cairo](http://www.cairographics.org/)

## Ubuntu

Building on Ubuntu requires the boost development libaries and the pkg-config tool:

```bash
$ sudo apt-get install libboost-all-dev
$ sudo apt-get install pkg-config
```

## OS X

Dependencies can be installed on OS X using homebrew:

```bash
$ brew install freetype
$ brew install libpng
$ brew install libjpeg
$ brew install boost
$ brew install gcc47
```

If you plan to generate nice HTML code coverage reports, you'll also want lcov:

```bash
$ brew install lcov
```

# Compiling

A standard set of the GNU autotools (autoconf, automake, libtool, etc) is required for configuration.  

```bash
$ ./autogen.sh
$ ./configure
$ make
```

## Configuration Options

There are several optional options which can be passed to the configure script.

* --enable-gcov   		- enable code coverage data to be generated
* --enable-debug  		- enable the unit tests to be built
* --enable-static-boost - statically link the boost library dependencies in the application


You'll most likely want to use the gcov and debug options in development.  
The static boost option may be useful for production builds where you don't want to have the separate boost dependencies installed.


# Usage

Since font-awesome was made to interface with the PHP website,
it takes its input through stdin, and dumps out the PNG file 
through STDOUT.

You can still test it on the command line using:


```bash
    $ cat "Hello world" | ./font-awesome -f in_file.ttf -s 36 > output.png
```

You can test the unicode capabilities using:

```bash
    $ cat tests/data/unicode_sample.txt | ./src/font-awesome --filename tests/data/2.otf --fontsize 62 --verbose > output.png
```

## CLI Options


### Text Input

The text to be rendered should be piped into the tool via STDIN.  The default input text mode is regular text strings.  If the _--codepoints_ flag is used then the input should be a series of unicode code points instead.  Each code point should be specified on a separate line.


## Rendering Options

The _--gracefulempty_ option can be used to force a 1x1 pixel transparent PNG to be output in cases when no text can actually be rendered.  The _--fix0glyph_ flag instructs the tool to draw a rectangle representing missing character glyphs in the input string.  This is useful for fonts which do not have their own missing glyph character defined.


### Backends

Text can be rendered using either one of two different backends chosen using the _--backend_ argument.  Valid values are _pixel_ and _cairo_.  The pixel rendering backend is the original implementation.  It simply copies rasterized FreeType glyphs directly into an image buffer.  Kerning and advanced multi-glyph ligature features are not supported by this backend.  The Cairo backend should be used if you need to support these font features.


### Image Output

The default mode is to send the PNG image data directly to STDOUT.  In some situations it may be more convenient to write the image directly to a file rather than piping or redirecting the output.  In those cases the _--outfile_ argument can be passed with the output image filename to be written to instead.


### Text Color

The text color can be controlled via the _--color_ and _--emptycolor_ options.  The arguments should be hex formatted RGBA color values.  The _--emptycolor_ option can be used to control the color used to render missing character glyphs.  By default missing glyphs will be rendered using the same color as normal characters.


### OpenType Features

The Cairo backend supports enabling different OpenType feature tags available in fonts via the _--feature_ flag.  This flag can be used multiple times, once per (Feature Tag)[http://partners.adobe.com/public/developer/opentype/index_tag3.html] you want to use.  The available features are enumerated within the "opentype_features" section of th _--metrics_ option's output.

### Metrics

There are several options for querying information about fonts.  The _--metrics_ flag will switch from outputting image data to displaying information about the text input string and the font itself.  The output information includes the total number of glyphs in the font and the count of characters in the text input which have corresponding renderable glyphs.  The _--glyphinfo_ option can be used to display information about a single glyph corresponding to the option's argument.  When paired with the _--showempty_ flag, information about the missing glyph will be displayed instead of the specified character glyph.  The format of the output data is either JSON or XML depending on whether the _--json_ or _--xml_ flag is used.  If the _--codepoints_ flag is included then the character map data will list the Unicode code points instead of the actual characters.

# Packaging

Ubuntu packages can be built:


```sh
sudo apt-get install build-essential devscripts debhelper
mv font-awesome font-awesome-1.0.2
cd font-awesome-1.0.2/
./autogen.sh
cd ..
tar czvf font-awesome_1.0.2.orig.tar.gz font-awesome-1.0.2/
cd font-awesome-1.0.2/
dch --create -v 1.0.2-1 --package font-awesome
debuild -us -uc
```

# Issues

* Fix Python version (it used to segfault, and now it doesn't but it also no longer works right)
* On OS X, if there are runtime crash issues with boost and libc, it may be necessary to install boost from source instead of relying on the prepackaged homebrew version.


# License

MIT


# Contributors


* [Shu Zong Chen](http://freelancedreams.com/)
* [Josh Farr](http://www.creativemarket.com/)


Extra m4 sugar from:

* https://github.com/RhysU/autoconf-boost.m4
* http://github.com/tsuna/boost.m4
