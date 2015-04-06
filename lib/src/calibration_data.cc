#include "dove_eye/calibration_data.h"

namespace dove_eye {

/**
 * @note Camera parameters might be calculated more accurately for more than
 * two cameras (i.e. take multiple camera pairs into account at once).
 */
void CalibrationData::CalculateGlobals() const {
  /* First camera position is given directly */
  rotations_[0] = rotation().inv();
  translations_[0] = -rotation() * position();

  /* Other cameras are taken relatively to the first (0-th) camera. */
  for (CameraIndex cam = 1; cam < Arity(); ++cam) {
    auto &pair = pair_parameters(cam - 1);

    rotations_[cam] = pair.rotation * rotations_[0];
    translations_[cam] = pair.rotation * translations_[0] + pair.translation;
  }

  /* Compose 3x4 projection matrices of cameras (from intrinsic and extrinsic) */
  for (CameraIndex cam = 0; cam < Arity(); ++cam) {
    cv::Mat projection(3, 4, CV_64F);

    for(int col = 0; col < 3; ++col) {
      rotations_[cam].col(col).copyTo(projection.col(col));
    }
    translations_[cam].copyTo(projection.col(3));

    projection = camera_parameters(cam).camera_matrix * projection;
    projections_[cam] = projection;
  }
}

} // end namespace dove_eye
