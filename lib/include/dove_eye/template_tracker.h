#ifndef DOVE_EYE_TEMPLATE_TRACKER_H_
#define DOVE_EYE_TEMPLATE_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/inner_tracker.h"
#include "dove_eye/kalman_filter.h"
#include "dove_eye/parameters.h"

namespace dove_eye {

class TemplateTracker : public InnerTracker {
 public:
  struct TemplateData : public TrackerData {
    cv::Mat search_template;
    double radius;

    /**
     * @param   point   position of template object
     * @return          top-left corner of the template
     */
    inline cv::Point TopLeft(const cv::Point point = cv::Point(0, 0)) const {
      return point - cv::Point(radius, radius);
    }

    /**
     * @param   point   position of template object
     * @return          bottom-right corner of the template
     */
    inline cv::Point BottomRight(const cv::Point point = cv::Point(0, 0)) const {
      return point + cv::Point(radius, radius);
    }
  };

  explicit TemplateTracker(const Parameters &parameters)
      : InnerTracker(parameters),
        initialized_(false) {
  }

  inline const TrackerData *tracker_data() const {
    return &data_;
  }
 
  bool InitializeTracking(const Frame &frame, Posit *result) override;

  bool InitializeTracking(
      const Frame &frame,
      const Epiline epiline,
      const TrackerData *tracker_data,
      Posit *result) override;
 
  bool Track(const Frame &frame, Posit *result) override;

  bool ReinitializeTracking(const Frame &frame, Posit *result) override;

  InnerTracker *Clone() const override;

 private:
  TemplateData data_;
  KalmanFilter<Point2> kalman_filter_;
  
  bool initialized_;

  bool TakeTemplate(const cv::Mat &data, const Point2 point, const double radius);

  bool Match(
      const cv::Mat &data,
      const TemplateData &tpl,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Point2 *result,
      double *quality = nullptr) const;

};

} // namespace dove_eye

#endif // DOVE_EYE_TEMPLATE_TRACKER_H_
