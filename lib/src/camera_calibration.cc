#include "dove_eye/camera_calibration.h"

#include <cassert>
#include <vector>

#include <dove_eye/logging.h>

using cv::calibrateCamera;
using cv::Point3f;
using cv::stereoCalibrate;
using std::vector;

namespace dove_eye {

CameraCalibration::CameraCalibration(const CameraIndex cameraCount,
                                     const CalibrationPattern &pattern) :
 cameraCount_(cameraCount),
 pattern_(pattern),
 imagePoints_(cameraCount),
 cameraStates_(cameraCount, kUnitialized),
 cameraParameters_(cameraCount),
 pairStates_(cameraCount, kUnitialized),
 pairParameters_(cameraCount),
 pairs_(CameraPair::GenerateArray(cameraCount)) {

}

bool CameraCalibration::MeasureFrameset(const Frameset &frameset) {
  assert(frameset.Size() == cameraCount_);
  bool result = true;

  /* 
   * First we search for pattern in each single camera,
   * when both camera from a pair are calibrated, we estimate pair parameters.
   */
  for(CameraIndex cam = 0; cam < cameraCount_; ++cam) {
    if (!frameset.IsValid(cam)) {
      result = result && (cameraStates_[cam] == kReady);
      continue;
    }
    
    Point2Vector imagePoints;

    switch (cameraStates_[cam]) {
      case kUnitialized:
      case kCollecting:
        if (pattern_.Match(frameset[cam].data, imagePoints)) {
          imagePoints_[cam].push_back(imagePoints);
          cameraStates_[cam] = kCollecting;
        }

        if (imagePoints_[cam].size() >= framesToCollect_) {
          vector<Point3Vector> objectPoints(imagePoints_[cam].size(),
              pattern_.ObjectPoints());

          auto error = calibrateCamera(objectPoints, imagePoints_[cam],
              frameset[cam].data.size(),
              cameraParameters_[cam].cameraMatrix,
              cameraParameters_[cam].distortionCoefficients,
              cv::noArray(), cv::noArray());
          DEBUG("Camera %i calibrated, reprojection error %f\n", cam, error);

          cameraStates_[cam] = kReady;
          imagePoints_[cam].clear(); /* Will use it for pair calibration */
        }

        result = false;
        break;
      case kReady:
        /* nothing to do here */
        break;
      default:
        assert(false);
    }
  }

  for (auto pair : pairs_) {
    auto cam1 = pair.cam1;
    auto cam2 = pair.cam2;

    if (cameraStates_[cam1] != kReady || cameraStates_[cam2] != kReady) {
      result = result && (pairStates_[pair.index] == kReady);
      continue;
    }
    if (!frameset.IsValid(cam1) || !frameset.IsValid(cam2)) {
      result = result && (pairStates_[pair.index] == kReady);
      continue;
    }

    Point2Vector imagePoints1, imagePoints2;
    cv::Mat dummy_R, dummy_T;

    switch (pairStates_[pair.index]) {
      case kUnitialized:
      case kCollecting:
        if (pattern_.Match(frameset[cam1].data, imagePoints1) &&
            pattern_.Match(frameset[cam2].data, imagePoints2)) {
          imagePoints_[cam1].push_back(imagePoints1);
          imagePoints_[cam2].push_back(imagePoints2);

          pairStates_[pair.index] = kCollecting;
        }
        DEBUG("Matching pair %i, %i\n", cam1, cam2);

        /* We add frames in lockstep, thus read size from cam1 only. */
        if (imagePoints_[cam1].size() >= framesToCollect_) {
          DEBUG("Calibrating pair %i, %i\n", cam1, cam2);
          vector<Point3Vector> objectPoints(imagePoints_[cam1].size(),
              pattern_.ObjectPoints());

          // FIXME Getting size more centrally probably.
          auto error = stereoCalibrate(objectPoints, imagePoints_[cam1],
              imagePoints_[cam2],
              cameraParameters_[cam1].cameraMatrix,
              cameraParameters_[cam1].distortionCoefficients,
              cameraParameters_[cam2].cameraMatrix,
              cameraParameters_[cam2].distortionCoefficients,
              cv::Size(1, 1), /* not actually used as we already know camera matrix */
              dummy_R,
              dummy_T,
              pairParameters_[pair.index].essentialMatrix, /* E */
              cv::noArray()); /* F */

          DEBUG("Pair %i, %i calibrated, reprojection error %f\n", cam1, cam2,
                error);

          pairStates_[pair.index] = kReady;
          imagePoints_[cam1].clear(); 
          imagePoints_[cam2].clear();
        }

        result = false;
        break;
      case kReady:
        /* nothing to do here */
        break;
      default:
        assert(false);
    }
  }

  return result;
}

}
