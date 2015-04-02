#ifndef DOVE_EYE_CALIBRATION_DATA_H_
#define DOVE_EYE_CALIBRATION_DATA_H_

#include <cassert>
#include <vector>

#include "dove_eye/camera_pair.h"
#include "dove_eye/types.h"

#ifdef HAVE_GUI
/* Forwards */
class QString;
#include "io/calibration_data_storage.h"
#endif

namespace dove_eye {

struct CameraParameters {
  cv::Mat camera_matrix;
  cv::Mat distortion_coefficients;
};

struct PairParameters {
  cv::Mat fundamental_matrix;
  cv::Mat rotation;
  cv::Mat translation;
};


class CalibrationData {
  friend class CameraCalibration;
#ifdef HAVE_GUI
  friend CalibrationData io::CalibrationDataStorage::LoadFromFile(const QString &);
#endif

 public:
  explicit CalibrationData(const CameraIndex arity = 0)
      : arity_(arity),
        camera_parameters_(arity),
        pair_parameters_(arity) {
  }

  inline const CameraParameters &camera_result(const CameraIndex cam) const {
    assert(cam < Arity());

    return camera_parameters_[cam];
  }

  inline const PairParameters &pair_result(const CameraIndex index) const {
    assert(index < CameraPair::Pairity(Arity()));

    return pair_parameters_[index];
  }
  
  inline CameraIndex Arity() const {
    return arity_;
  }

 private:
  // FIXME Should be const, was lazy to implement the-big-five (operator=, ...)
  CameraIndex arity_;

  std::vector<CameraParameters> camera_parameters_;
  std::vector<PairParameters> pair_parameters_;
};

} // namespace dove_eye

#ifdef HAVE_GUI
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::CalibrationData)
#endif

#endif // DOVE_EYE_CALIBRATION_DATA_H_

