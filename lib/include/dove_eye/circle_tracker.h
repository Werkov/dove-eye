#ifndef DOVE_EYE_CIRCLE_TRACKER_H_
#define DOVE_EYE_CIRCLE_TRACKER_H_

#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/searching_tracker.h"

namespace dove_eye {

class CircleTracker : public SearchingTracker {
 public:
  struct CircleData : public TrackerData {
    const int histogram_size = 32;
    /* Hue range is float because of calcBackProject API */
    float hrange[2];
    double srange[2];
    double vrange[2];

    cv::Mat histogram;
    double radius;

    CircleData() {
      /* Because of MSVC bug with error C2536, use struct ctor. */
      hrange[0] = 0;
      hrange[1] = 180;
    }
  };

  explicit CircleTracker(const Parameters &parameters)
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

    return new CircleTracker(*this);
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
    const auto &data = static_cast<const CircleData &>(tracker_data);

    cv::Size new_size(2 * data.radius * f, 2 * data.radius * f);
    return cv::Rect(exp - 0.5 * Point2(new_size), new_size);
  }

 private:
  typedef cv::Vec3f Circle;
  typedef std::vector<Circle> CircleVector;

  CircleData data_;
  
  bool UpdateData(CircleData &circle_data, const cv::Mat &data,
                  const Mark &mark) const;

  cv::Mat PreprocessImage(const cv::Mat &data,
                          const CircleData &circle_data,
                          const double threshold) const;

  double CirclesToMark(const cv::Mat& data,
                     const CircleVector &circles,
                     Mark *mark) const;
};

} // namespace dove_eye

#endif // DOVE_EYE_CIRCLE_TRACKER_H_
