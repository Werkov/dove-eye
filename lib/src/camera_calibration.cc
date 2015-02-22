#include "dove_eye/camera_calibration.h"

#include <cassert>
#include <vector>

#include <dove_eye/logging.h>

using cv::calibrateCamera;
using cv::Point3f;
using std::vector;

namespace dove_eye {

CameraCalibration::CameraCalibration(const CameraIndex cameraCount,
                                     const CalibrationPattern &pattern) :
 cameraCount_(cameraCount),
 pattern_(pattern),
 imagePoints_(cameraCount),
 cameraStates_(cameraCount, kUnitialized),
 cameraParameters_(cameraCount) {

}

bool CameraCalibration::MeasureFrameset(const Frameset &frameset) {
  // FIXME So far only camcalib from the first camera.
  const int cam = 0;
  assert(cam < cameraCount_);
  bool result = false;
  Point2Vector imagePoints;

  DEBUG("measuring\n");
  if(!frameset.IsValid(cam)) {
    return result;
  }
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

        // FIXME Getting size more centrally probably.
        auto error = calibrateCamera(objectPoints, imagePoints_[cam],
            frameset[cam].data.size(),
            cameraParameters_[cam].cameraMatrix,
            cameraParameters_[cam].distortionCoefficients,
            cv::noArray(), cv::noArray());
        DEBUG("Camera %i, reprojection error %f\n", cam, error);

        cameraStates_[cam] = kReady;
        result = true;
      }
      break;
    case kReady:
      return true;
      break;
  }
  return result;
}

}
