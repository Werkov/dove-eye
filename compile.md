---
layout: page
title: Compile
---

## Linux

The `dove-eye` requires some 3rd party libraries: 

  * [libQGLViewer](http://www.libqglviewer.com/), OpenGL,
  * OpenCV 3 (2.4 worked as well),
  * Qt5.

When you have the libraries installed, `dove-eye` is built using `cmake`.

    $ git clone git@github.com:Werkov/dove-eye.git
    $ cd dove-eye
    $ mkdir build && cd build
    $ cmake .. 
    $ make

Please note that the snippet above assumes, you have all libraries installed in
system paths. (You may alternatively use `cmake` parameter
`-DQGLViewer_ROOT=<path to your installation of libQGLViewer>`.)

### libQGLViewer compilation

  * download libQGLViewer sources,
  * make sure you have `qt5-qmake` installed and in `$PATH` (check `qmake --version`),
    * If you happened to build libQGLViewer with Qt4, you have to `make
      distclean` and manually remove `*.so` built files.
  * run `qmake *.pro && make` to build your version of libQGLViewer
    * some make targets may fail, it's only important to have `libQGLViewer.so`
      successfully built.

With local version of libQGLViewer you need to set cmake accordingly

    $ cmake -DQGLViewer_ROOT=<path to dir where QGLViewer/libQGLViewer.so is found>

#### Packages in Ubuntu

Ubuntu since 18.04 ships the libQGLViewer as package `libqglviewer-dev-qt5`.
(libQGLViewer that is packaged in earlier versions is linked agaist Qt4 --
despite you can build `dove-eye`, it'll segfault due to ABI incompatibilities!)

#### Packages in openSUSE

You can use a user-built RPMs from [`home:mkoutny:dove-eye/libQGLViewer`][viewer]

[viewer]: https://build.opensuse.org/package/show/home:mkoutny:dove-eye/libQGLViewer

## Windows (applies to dove-eye 0.1)

Download and install compiled binaries for OpenCV 2.4 (not 3.0),
do the same for Qt framework, version 5.

Then you must download and install Qt Creator. Once you have it,
download sources for libQGLViewer library and compile it using the Qt Creator.

Now you have dependency libraries ready and you can compile `dove-eye` itself.
Start cmake-gui and open `CMakeLists.txt` in the root directory of the
`dove-eye` project.
You need to set proper paths to the libraries above, so that CMake can find
them during configure step. When configuration succeeds you can generate a
Visual Studio project.

[Screenshot of configured CMake project](https://raw.githubusercontent.com/Werkov/dove-eye/master/doc/win-cmake-configured.png)

The project can be then normally built in VS (FIXME probably, I don't remember
any problems now, it's been a while).

NOTE: I had some problems with debug versions of system libraries and 32b
builds on Windows 7.
