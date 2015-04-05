#ifndef DOVE_EYE_INNER_TRACKER_H_
#define DOVE_EYE_INNER_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/frame.h"
#include "dove_eye/positset.h"
#include "dove_eye/types.h"

namespace dove_eye {

struct TrackerData {
};

class InnerTracker {
 public:
  /** Mark is an object in input frame that can initialize tracking */
  typedef Point2 Mark;
  typedef Vector3 Epiline;

  virtual void SetMark(const Mark mark) = 0;

  virtual const TrackerData *tracker_data() const = 0;

  /** Global initialization from mark */
  virtual bool InitializeTracking(const Frame &frame, Posit *result) = 0;
  
  /** Epiline initialization from other tracker's data */
  virtual bool InitializeTracking(
      const Frame &frame,
      const Epiline epiline,
      const TrackerData *tracker_data,
      Posit *result) = 0;

  virtual bool Track(const Frame &frame, Posit *result) = 0;

  /** Global reinitialization */
  virtual bool ReinitializeTracking(const Frame &frame, Posit *result) = 0;

  /** Reinitialization from epiline */
  virtual inline bool ReinitializeTracking(const Frame &frame,
                                    const Epiline epiline,
                                    Posit *result) {
    return ReinitializeTracking(frame, result);
  }

  /** Reinitalization from projection point guess */
  virtual inline bool ReinitializeTracking(const Frame &frame,
                                    const Point2 guess,
                                    Posit *result) {
    return ReinitializeTracking(frame, result);
  }


  virtual InnerTracker *Clone() const = 0;
};

} // namespace dove_eye

#endif // DOVE_EYE_INNER_TRACKER_H_

