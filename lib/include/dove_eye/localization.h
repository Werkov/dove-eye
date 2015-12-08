#ifndef DOVE_EYE_LOCALIZATION_H_
#define DOVE_EYE_LOCALIZATION_H_

#include "dove_eye/calibration_data.h"
#include "dove_eye/camera_pair.h"
#include "dove_eye/location.h"
#include "dove_eye/positset.h"
#include "dove_eye/types.h"

namespace dove_eye {

class Localization {
 public:
  explicit Localization(const CameraIndex arity)
      : arity_(arity),
        pairs_(CameraPair::GenerateArray(arity_)),
        calibration_data_(nullptr) {
  }

  inline CameraIndex Arity() const {
    return arity_;
  }

  inline CameraIndex PositsRequired() const {
    return 2;
  }

  inline void calibration_data(const CalibrationData *value) {
    calibration_data_ = value;
  }

  inline const CalibrationData* calibration_data() const {
    return calibration_data_;
  }

  bool Locate(const Positset &positset, Location *result);

 private:
  const CameraIndex arity_;
  const CameraPair::PairArray pairs_;

  const CalibrationData *calibration_data_;

  Location PairLocate(const Positset &positset, const CameraPair pair);
};

} // namespace dove_eye

#endif // DOVE_EYE_LOCALIZATION_H_

