#ifndef DOVE_EYE_CAMERA_PAIR_H_
#define DOVE_EYE_CAMERA_PAIR_H_

#include <vector>

#include "dove_eye/types.h"

namespace dove_eye {

struct CameraPair {
  typedef std::vector<CameraPair> PairArray;

  CameraIndex cam1;
  CameraIndex cam2;
  CameraIndex index;

  static PairArray GenerateArray(const CameraIndex arity);

  static CameraIndex Index(const CameraIndex arity,
                           const CameraIndex cam1,
                           const CameraIndex cam2);

  static CameraIndex Pairity(const CameraIndex arity);
};

} // namespace dove_eye

#endif // DOVE_EYE_CAMERA_PAIR_H_

