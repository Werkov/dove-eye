---
layout: page
title: Developers
---

*Note on terminology: Position means 2D coordinates of the object projection in
the image. Location denotes 3D coordinates of the object in the world.
Detection of position and location is tracking and localization respectively.*

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

Currently, the selection of the tracker is done at compile time (in
`Application::SetupController`) and the application is compiled with
<del>`dove_eye::HistogramTracker`</del>`dove_eye::CircleTracker` by default. It
experimentally proved to be more stable.

Third tracker is `dove_eye::CircleTracker`, that uses similar hue filtering as
the `HistogramTracker`, however, it doesn't detect blobs but circles of the
given color. In the end, it is subjectively as (un)stable as the
`HistograTracker`.

### Tracker tuning

(See `lib/src/parameters.cc` for source.)

  * `track.template.radius`
    * used as radius of Gauss kernel for denoising (`CircleTracker`, `HistogramTracker`)
    * (experimental, obsolete) used as epiline thickness when predicing on epiline
  * `track.search.factor`
    * size of searched area in terms of previously matched pattern (i.e. you
      track a circle with radius `r`, however, on the next frame ROI is 
      `factor * r` (optimization to avoid searching all image)
    * used by all trackers (that inherit from `SearchingTracker`)
  * `track.search.threshold`
    * [0, 1] value specifying required quality of tracking
    * `TemplateTracker` (1 = accept only good match)
    * `HistogramTracker` (1 = accept only strongest histogram backprojection)
    * `CircleTracker` *unused*
  * `track.search.min_speed`
    * minimum speed to apply background subtraction
    * *currently unused*
  * `track.search.kf.proc_v`
    * process variation for prediction Kalman filter
  * `track.search.kf.obs_v`
    * observation variation for observation Kalman filter
    * consider ratio over `track.search.kf.proc_v` -- when it's large
      observation are taken into account with small weight only, as result
      prediction can be delayed behind actual state.

### Tracking quality

Since tracking quality (and calibration) substantially affects resulting
location, it's (now) obvious that it's the critical part of the system. There's
space for experimenting with various implementations of the
`dove_eye::InnerTracker` (abstract) class and possibly improve the state
machine in the wrapping `dove_eye::Tracker` class.

