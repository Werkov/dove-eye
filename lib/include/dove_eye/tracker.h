#ifndef DOVE_EYE_TRACKER_H_
#define DOVE_EYE_TRACKER_H_

#include <memory>
#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_data.h"
#include "dove_eye/frame.h"
#include "dove_eye/frameset.h"
#include "dove_eye/inner_tracker.h"
#include "dove_eye/location.h"
#include "dove_eye/positset.h"

namespace dove_eye {

/**
 * @note This class is not (intentionaly) thread safe, i.e. can be used in
 *       single thread only.
 */
class Tracker {
 public:
  explicit Tracker(const CameraIndex arity, const InnerTracker &inner_tracker);

  Positset SetMark(const Frameset &frameset, const CameraIndex cam,
               const InnerTracker::Mark mark, bool project_other = false);

  bool SetLocation(const Location location);

  Positset Track(const Frameset &frameset);

  inline bool distorted_input() const {
    return distorted_input_;
  }

  /**
   * When set to true, calibration data must be set too, so that tracker can
   * undistort points.
   */
  inline void distorted_input(bool value) {
    distorted_input_ = value;
  }

  inline const CalibrationData *calibration_data() const {
    return calibration_data_;
  }

  inline void calibration_data(const CalibrationData *value) {
    calibration_data_ = value;
  }

 private:
  enum TrackState {
    kUninitialized,
    kTracking,
    kLost
  };

  typedef std::vector<TrackState> StateVector;
  typedef std::unique_ptr<InnerTracker> InnerTrackerPtr;
  typedef std::vector<InnerTrackerPtr> TrackerVector;

  const CameraIndex arity_;

  /** Output */
  Positset positset_;

  StateVector trackstates_;

  TrackerVector trackers_;

  bool distorted_input_;

  const CalibrationData *calibration_data_;

  Location location_;
  bool location_valid_;

  bool TrackSingle(const CameraIndex cam, const Frame &frame);

  Point2 Undistort(const Point2 &point, const CameraIndex cam) const;

  InnerTracker::Epiline CalculateEpiline(
      const Posit posit,
      const CameraIndex marked_cam,
      const CameraIndex cam) const;

  Point2 ReprojectLocation(const Location location, const CameraIndex cam) const;
};

} // namespace dove_eye

#endif // DOVE_EYE_TRACKER_H_

