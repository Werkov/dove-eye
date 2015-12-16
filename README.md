Maybe you've heard about [Hawk-Eye](https://en.wikipedia.org/wiki/Hawk-Eye),
a system that tracks a tennis ball with immense precision using multiple cameras.

Dove-eye is an experimental project that attempts to track visually distinctive
object with multiple consumer-grade webcams and reconstruct its 3D location.

[![Tracking a colorful object on border of paper](http://img.youtube.com/vi/yErHmSeo49E/0.jpg)](https://www.youtube.com/watch?v=yErHmSeo49E)

# Demo and manual

*Note on terminology: Position means 2D coordinates of the object projection in
the image. Location denotes 3D coordinates of the object in the world.
Detection of position and location is tracking and localization respectively.*

## Cameras

  1. First setup your cameras so that they capture your scene of interest.

  2. Then start `dove-eye` and in menu choose **Providers**, there you set up
     connected cameras.
    * Cameras are identified by device indices.
    * You may run into problems (*No space left on device*) if you have a single
      USB controller and use large resolution of cameras. For this reason, I
      recommend reducing resolution to 320 x 240, which works even for three
      USB webcams on one controller.

  3. After successful setup, you'd see streams from all three cameras and you
     can proceed to calibration.
    * (For absolute scale, measure size of a chessboard pattern and update it
      in parameters via **Parameters** -> **Modify** -> **calibration.***. The
      size is in meters and you can load/store the parameters into file.)
    * Choose **Calibration** -> **Calibrate** in menu, you'd see progress of
      calibration in the status bar.
    * First, each single camera is calibrated and camera pairs are calibrated
      afterwards (i.e. you should "show" chessboard pattern first to each
      camera and then to each pair of them).
    * Calibration completion is indicated in the status bar.
    * I recommend to save the calibration parameters to a file so that you
      don't have to repeatedly calibrate the same setup.

  4. Now you only have to point to the tracked object.
    * In each video window, "draw a rectangle" around the visually distinctive
      object to initialize the tracker. You would see a red circle around the
      recognized object.

  5. Start localization after object is tracked in all cameras
    * Push **Localization** -> **Start** and you would see a dot in the 3D
      viewer which depicts location of the object in the world.
    * As you move the object, you would see its trajectory drawn.
    * Visual tracking is the most vital part of the system, if the tracked
      object is lost, localization would output nonsensical results.
    * TODO format of output data

## Video files

  1. Capture videos of your scene, beginning should contain calibration pattern
     if you don't have cameras calibrated previously (assuming videos are
     time-synchronized).

  2. In menu **Providers** choose **Video file(s)**, choose your video files
     (must be valid videos, other fails cause crash :-).

  3. You won't see the videos playing, click **Step** few times, you should see
     first frames of the videos.

  4. The rest is similar as work with cameras. Video playback is currently
     intended for step-by-step processing only, **Play** button doesn't
     implement FPS correctly.

## Sample calibration

[![Calibration using chessboard pattern](http://img.youtube.com/vi/dvVXtbc0moI/0.jpg)](https://www.youtube.com/watch?v=dvVXtbc0moI)


# Installation

## Ubuntu

There's a PPA repository that packages the application. The installation is as
simple as follows.

    $ sudo add-apt-repository ppa:werkov/ppa
    $ sudo apt-get update
    $ sudo apt-get install dove-eye

    $ # after install just launch it
    $ dove-eye

Currently, [there are builds][pkg] for Ubuntu 14.04 Trusty and Ubuntu 15.04 Vivid.

[pkg]: https://github.com/Werkov/dove-eye-installer/blob/master/README.md

## Windows

TODO


# Compilation

## Linux

The `dove-eye` requires some 3rd party libraries: 

  * [libQGLViewer](http://www.libqglviewer.com/), OpenGL,
  * OpenCV,
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

### Gotchas in Ubuntu

libQGLViewer that is packaged in Ubuntu (14.\*, 15.04) is linked agaist Qt4 --
you might build `dove-eye`, however, it'll segfault during ABI
incompatibilities! Thus it is recommended to compile libQGLViewer locally with
Qt5 libraries.

  * download libQGLViewer sources,
  * make sure you have `qt5-qmake` installed and in `$PATH` (check `qmake --version`),
    * If you happened to build libQGLViewer with Qt4, you have to `make
      distclean` and manually remove `*.so` built files.
  * run `qmake *.pro && make` to build your version of libQGLViewer
    * some make targets may fail, it's only important to have `libQGLViewer.so`
      successfully built.

With local version of libQGLViewer you need to set cmake accordingly

    $ cmake -DQGLViewer_ROOT=<path to dir where libQGLViewer/libQGLViewer.so is found>

Other libraries can be installed from packages: TODO (note -dev versions).

### Gotchas in openSUSE

These packages are required:

  * opencv-qt5-devel (must -qt5- vesion),
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

# For developers

Application `dove-eye` consists of three layers: general tracking and
localization library (namespace `dove_eye`), various widgets for GUI (namespace
`gui`) and glue layer of QObjects (top unnamed namespace).

## dove_eye library

The dove_eye library is based on a set of objects that are connected to a
pipeline, the basic setup for tracking and localization is on the following
figure.

          VideoProvider    ...   VideoProvider
                |                      |
             (Frame)       ...      (Frame)
                \                      /
                 v                    v
                    FramesetAggregator
                            |
                       (Frameset)
                            |
                            +--> to GUI
                            |
                            v
                         Tracker (InnerTracker for each camera)
                            |
                       (Positset)
                            |
                            v
                       Localization
                            |
                       (Location)
                            |
                            +--> to GUI
                            |


## Control QObjects

Communication between dove_eye library and GUI is backed by several
`QObject`s, most importantly:

  * `Controller` class realizes the pipeline illustrated above. Basically, it
    runs a loop that iterates throught the pipeline and it provides the data
    from the pipeline to others via Qt signal-slot mechanism.

  * `FramesetConverter` class ensures near real-time video display.

  * `Application` class keeps general context and its task is to correctly
    initialize, connect and start the `Controller`, the `FramesetConverter` and
    the GUI.


## Threading

There are several threads running in the application:

  * GUI thread (the initial thread) -- it redraws basic widgets, collects and
    dispatches GUI events,
  * `Controller`'s thread -- it runs the tracking/localization pipeline,
  * `FramesetConverter`'s thread -- it (asynchronously) draws frames that it
    obtains via `Controller` from the pipeline,
  * `FramesetAggregator`'s threads -- `FramesetAggregator` runs a single thread
    for each camera, so that it's possible to grab videos from multiple streams
    even though the API is inherently blocking.

Any communication between different threads must be done only via Qt's
signal-slot mechanism, which ensures implicit synchronization, furthermore
OpenCV `cv::Mat` class is also thread-safe and can be shared among threads.
Otherwise explicit synchronization must be done (e.g. in `FramesetAggregator`).

## Tracking and localization

Actual localization is simply delegated to OpenCV library functions and thus
merit of `dove-eye` lies in tracking.

As shown in the pipeline scheme, there's a wrapping class `dove_eye::Tracker`
that only keeps state of tracking in individual video streams and the actual
tracking is implemented in `dove_eye::InnerTracker` class descendants. There is
a specialization (`dove_eye::SearchingTracker`) that contains frequently
reused code for trackers that work by searching an object in an image in
neighborhood of a predicted position.

### Implemented trackers

There is a `dove_eye::HistogramTracker` that employs [backprojection][1] to
detect object according to its color (spectrum). Second implementation
`dove_eye::TemplateTracker` utilize [template matching][2] to detect the most
similar object to an initial pattern. Both these implementation share code for
background subtraction from `dove_eye::SearchingTracker`.

[1]: http://docs.opencv.org/doc/tutorials/imgproc/histograms/back_projection/back_projection.html?highlight=backprojection
[2]: http://docs.opencv.org/doc/tutorials/imgproc/histograms/template_matching/template_matching.html?highlight=template

Currently, the selection of the tracker is done at compile time and the
application is compiled with `dove_eye::HistogramTracker` by default. It
experimentally proved to be more stable.

Third tracker is `dove_eye::CircleTracker`, that uses similar hue filtering as
the `HistogramTracker`, however, it doesn't detect blobs but circles of the
given color. In the end, it is subjectively as (un)stable as the
`HistograTracker`.

### Tracking quality

Since tracking quality (and calibration) substantially affects resulting
location, it's (now) obvious that it's the critical part of the system. There's
space for experimenting with various implementations of the
`dove_eye::InnerTracker` (abstract) class and possibly improve the state
machine in the wrapping `dove_eye::Tracker` class.




