#ifndef DOVE_EYE_TEMPLATE_TRACKER_H_
#define DOVE_EYE_TEMPLATE_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/inner_tracker.h"
#include "dove_eye/parameters.h"

namespace dove_eye {

class TemplateTracker : public InnerTracker {
 public:
  explicit TemplateTracker(const Parameters &parameters)
      : parameters_(parameters),
        initialized_(false) {
  }

  bool InitializeTracking(const Frame &frame, const Mark mark,
                          Posit *result) override;
  
  bool Track(const Frame &frame, Posit *result) override;

  InnerTracker *Clone() const override;

 private:
  const Parameters &parameters_;
  bool initialized_;

  cv::Mat template_;
  double radius_;
  cv::Point2f previous_match_;

};

} // namespace dove_eye

#endif // DOVE_EYE_TEMPLATE_TRACKER_H_
