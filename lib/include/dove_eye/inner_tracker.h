#ifndef DOVE_EYE_INNER_TRACKER_H_
#define DOVE_EYE_INNER_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/frame.h"

namespace dove_eye {

class InnerTracker {
 public:
  typedef cv::Point2f Mark;
  typedef cv::Point2f Posit;

  virtual bool InitializeTracking(const Frame &frame, const Mark mark,
                                  Posit *result) = 0;
  
  virtual bool Track(const Frame &frame, Posit *result) = 0;

  virtual InnerTracker *Clone() const = 0;
};

} // namespace dove_eye

#endif // DOVE_EYE_INNER_TRACKER_H_

