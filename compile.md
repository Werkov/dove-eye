---
layout: page
title: Compile
---

## Linux

The `dove-eye` requires some 3rd party libraries: 

  * [libQGLViewer](http://www.libqglviewer.com/), OpenGL,
  * OpenCV (2.4),
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

#### Gotchas in Ubuntu

libQGLViewer that is packaged in Ubuntu (14.\*, 15.04) is linked agaist Qt4 --
despite you can build `dove-eye`, it'll segfault due to ABI incompatibilities!
Thus it is recommended to compile libQGLViewer locally with Qt5 libraries.

#### Gotchas in openSUSE

These packages are required:

  * opencv-qt5-devel (must use opencv compiled against Qt5),
  * libqt5-qtbase-devel.

libQGLViewer has to be manually compiled with Qt5.

## Windows

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
