---
layout: page
title: Manual
---


## Cameras

  * First setup your cameras so that they capture your scene of interest.

  * Then start `dove-eye` and in menu choose **Providers**, there you set up
     connected cameras.
    * Cameras are identified by device indices.
    * You may run into problems (*No space left on device*) if you have a single
      USB controller and use large resolution of cameras. For this reason, I
      recommend reducing resolution to 320 x 240, which works even for three
      USB webcams on one controller.

  * After successful setup, you'd see streams from all three cameras and you
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

  * Now you only have to point to the tracked object.
    * In each video window, "draw a rectangle" around the visually distinctive
      object to initialize the tracker. You would see a red circle around the
      recognized object.

  * Start localization after object is tracked in all cameras
    * Push **Localization** -> **Start** and you would see a dot in the 3D
      viewer which depicts location of the object in the world.
    * As you move the object, you would see its trajectory drawn.
    * Visual tracking is the most vital part of the system, if the tracked
      object is lost, localization would output nonsensical results.
    * TODO format of output data

## Video files

  * Capture videos of your scene, beginning should contain calibration pattern
     if you don't have cameras calibrated previously (assuming videos are
     time-synchronized).

  * In menu **Providers** choose **Video file(s)**, choose your video files
     (must be valid videos, other fails cause crash :-).

  * You won't see the videos playing, click **Step** few times, you should see
     first frames of the videos.

  * The rest is similar as work with cameras. Video playback is currently
     intended for step-by-step processing only, **Play** button doesn't
     implement FPS correctly.

## Sample calibration

[![Calibration using chessboard pattern](http://img.youtube.com/vi/dvVXtbc0moI/0.jpg)](https://www.youtube.com/watch?v=dvVXtbc0moI)

