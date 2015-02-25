#ifndef DOVE_EYE_CAMERA_PAIR_H_
#define	DOVE_EYE_CAMERA_PAIR_H_

#include <vector>

#include <dove_eye/calibration_pattern.h>
#include <dove_eye/frameset.h>

namespace dove_eye {

struct CameraPair {
  typedef std::vector<CameraPair> PairArray;

  CameraIndex cam1;
  CameraIndex cam2;
  CameraIndex index;

  static PairArray GenerateArray(const CameraIndex cameraCount);
}

;

} // namespace dove_eye

#endif	/* DOVE_EYE_CAMERA_PAIR_H_ */

