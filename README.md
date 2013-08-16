# Overview

font-awesome is a command line tool to render TTF/OTF fonts to PNG.

# Dependencies

* [FreeType 2](http://www.freetype.org/freetype2/)
* [libpng](http://www.libpng.org/pub/png/libpng.html)
* [boost](http://www.boost.org/)


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

Since font-awesome was made to interface with the php website,
it takes its input through stdin, and dumps out the png file 
through stdout.

You can still test it on the command line using:

::

    $ cat "Hello world" | ./font-awesome -f in_file.ttf -s 36 > output.png


You can test the unicode capabilities using:

::

    $ cat unicode_sample.txt | ./font-awesome -f testfonts/5.otf -s 62 -v > output.png


# Issues 

* Fix Python version (it used to segfault, and now it doesn't but it also no longer works right)
* On OS X, if there are runtime crash issues with boost and libc, it may be necessary to install boost from source instead of relying on the prepackaged homebrew version.


# Contributors


* [Shu Zong Chen](http://freelancedreams.com/)
* [Josh Farr](http://www.creativemarket.com/)


Extra m4 sugar from:

* https://github.com/RhysU/autoconf-boost.m4 
* http://github.com/tsuna/boost.m4
