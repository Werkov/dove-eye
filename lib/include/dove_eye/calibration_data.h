#ifndef DOVE_EYE_CALIBRATION_DATA_H_
#define DOVE_EYE_CALIBRATION_DATA_H_

#include <cassert>
#include <vector>

#include <opencv2/opencv.hpp>

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


/**
 * CalibrationData object is immutable by design, only friends can update it
 */
class CalibrationData {
  friend class CameraCalibration;
#ifdef HAVE_GUI
  friend CalibrationData io::CalibrationDataStorage::LoadFromFile(const QString &);
#endif

 public:
  explicit CalibrationData(const CameraIndex arity = 0)
      : arity_(arity),
        camera_parameters_(arity_),
        pair_parameters_(arity_),
        globals_initialized_(false),
        rotations_(arity_),
        translations_(arity_),
        projections_(arity_) {
    position_ = cv::Mat::zeros(3, 1, CV_64F);
    rotation_ = cv::Mat::eye(3, 3, CV_64F);
  }

  inline const CameraParameters &camera_parameters(const CameraIndex cam) const {
    assert(cam < Arity());

    return camera_parameters_[cam];
  }

  inline const PairParameters &pair_parameters(const CameraIndex index) const {
    assert(index < CameraPair::Pairity(Arity()));

    return pair_parameters_[index];
  }

  inline const cv::Mat &position() const {
    return position_;
  }

  inline const cv::Mat &rotation() const {
    return rotation_;
  }

  inline CameraIndex Arity() const {
    return arity_;
  }

  inline const cv::Mat &CameraRotation(const CameraIndex cam) const {
    assert(cam < Arity());

    if (!globals_initialized_) {
      CalculateGlobals();
    }

    return rotations_[cam];
  }

  inline const cv::Mat &CameraTranslation(const CameraIndex cam) const {
    assert(cam < Arity());

    if (!globals_initialized_) {
      CalculateGlobals();
    }

    return translations_[cam];
  }

  inline const cv::Mat &ProjectionMatrix(const CameraIndex cam) const {
    assert(cam < Arity());

    if (!globals_initialized_) {
      CalculateGlobals();
    }

    return projections_[cam];
  }

 private:
  typedef std::vector<cv::Mat> CvMatVector;

  // FIXME Should be const, was lazy to implement the-big-five (operator=, ...)
  CameraIndex arity_;

  /** Position (in the world) of camera system, relative to camera 0 */
  cv::Mat position_;

  /** Rotation (camera-to-world) of camera system, relative to camera 0 */
  cv::Mat rotation_;

  std::vector<CameraParameters> camera_parameters_;
  std::vector<PairParameters> pair_parameters_;

  mutable bool globals_initialized_;
  mutable CvMatVector rotations_;
  mutable CvMatVector translations_;
  mutable CvMatVector projections_;


  inline void position(const cv::Mat &value) {
    position_ = value;
    globals_initialized_ = false;
  }

  inline void rotation(const cv::Mat &value) {
    rotation_ = value;
    globals_initialized_ = false;
  }

  void CalculateGlobals() const;
};

} // namespace dove_eye

#ifdef HAVE_GUI
#include <QMetaType>
Q_DECLARE_METATYPE(dove_eye::CalibrationData)
#endif

#endif // DOVE_EYE_CALIBRATION_DATA_H_

