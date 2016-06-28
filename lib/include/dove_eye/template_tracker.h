#ifndef DOVE_EYE_TEMPLATE_TRACKER_H_
#define DOVE_EYE_TEMPLATE_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/searching_tracker.h"

namespace dove_eye {

class TemplateTracker : public SearchingTracker {
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
      : SearchingTracker(parameters) {
  }

  inline const TrackerData &tracker_data() const override {
    return data_;
  }

  inline TrackerData &tracker_data() override {
    return data_;
  }

  InnerTracker *Clone() const override {
    assert(!initialized());

    return new TemplateTracker(*this);
  }

  inline InnerTracker::Mark::Type PreferredMarkType() const {
    return InnerTracker::Mark::kCircle;
  }

 protected:
  bool InitTrackerData(const cv::Mat &data, const Mark &mark) override;

  bool Search(
      const cv::Mat &data,
      TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result) const override;

  inline Posit MarkToPosit(const Mark &mark) const override {
    assert(mark.type == Mark::kCircle);
    return mark.center;
  }

  inline cv::Rect DataToRoi(const TrackerData &tracker_data, const Point2 exp,
                            const double search_factor) const override {
    const auto f = search_factor;
    const auto &data = static_cast<const TemplateData &>(tracker_data);
    return cv::Rect(exp.x - f * data.radius, exp.y - f * data.radius,
                    2 * f * data.radius, 2 * f * data.radius);
  }

 private:
  TemplateData data_;
};

} // namespace dove_eye

#endif // DOVE_EYE_TEMPLATE_TRACKER_H_
