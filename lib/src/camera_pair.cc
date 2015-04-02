#include "dove_eye/camera_pair.h"

#include <vector>

#include "dove_eye/calibration_pattern.h"
#include "dove_eye/frameset.h"

namespace dove_eye {

CameraPair::PairArray CameraPair::GenerateArray(const CameraIndex arity) {
  const CameraIndex pair_count = CameraPair::Pairity(arity);
  PairArray result(pair_count);

  int index = 0;
  for (int cam1 = 0; cam1 < arity; ++cam1) {
    for (int cam2 = cam1 + 1; cam2 < arity; ++cam2) {
      result[index].index = index;
      result[index].cam1 = cam1;
      result[index].cam2 = cam2;
    }
  }

  return result;
}

CameraIndex CameraPair::Index(const CameraIndex arity,
                              const CameraIndex cam1,
                              const CameraIndex cam2) {
  /* <- cam1 ->
   * c  .0123
   * a  ..456
   * m  ...78
   * 2  ....9
   *    .....
   *
   * Area of trapezoid (arithmetic sequence) + row index shifted by cam2.
   * Beware of zero-based indexing.
   */
  return (2 * arity - cam2 - 1) * (cam2 + 1) / 2 + (cam1 - cam2 - 1);
}

CameraIndex CameraPair::Pairity(const CameraIndex arity) {
  return arity * (arity - 1) / 2;
}

} // namespace dove_eye

