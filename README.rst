========
Overview
========

font-awesome is a command line tool to render TTF/OTF fonts to PNG.

Dependencies
============

font-awesome requires freetype, libpng, and maybe some unicode
library once I figure out how to fix the unicode problems.


Compiling
=========

The Makefile included is enough. Just:

::

    $ make

Okay, I assume you have all the build tools and the dependencies.

Usage
=====

Since font-awesome was made to interface with the php website,
it takes its input through stdin, and dumps out the png file 
through stdout.

You can still test it on the command line using:

::

    $ cat "Hello world" | font-awesome -f in_file.ttf -s 36 > output.png

Contributors
============

  * `Shu Zong Chen`_

.. CONTRIBUTORS

.. _`Shu Zong Chen`: http://freelancedreams.com/
