#ifndef DOVE_EYE_CAMSHIFT_TRACKER_H_
#define DOVE_EYE_CAMSHIFT_TRACKER_H_

#include <opencv2/opencv.hpp>

#include "dove_eye/searching_tracker.h"

namespace dove_eye {

class CamshiftTracker : public SearchingTracker {
 public:
  struct CamshiftData : public TrackerData {
    const int histogram_size = 16;
    /* Workaround for OpenCV 'const float **' API */
    float hrange[2] = {0, 180};

    cv::Mat histogram;
    cv::Size size;
    mutable cv::Rect track_window;
  };

  explicit CamshiftTracker(const Parameters &parameters)
      : SearchingTracker(parameters) {
  }

  inline const TrackerData &tracker_data() const override {
    return data_;
  }
 
  InnerTracker *Clone() const override {
    assert(!initialized());

    return new CamshiftTracker(*this);
  }

 protected:
  bool InitTrackerData(const cv::Mat &data, const Mark &mark) override;

  bool Search(
      const cv::Mat &data,
      const TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result,
      double *quality = nullptr) const override;

  inline Posit MarkToPosit(const Mark &mark) const override {
    assert(mark.type == Mark::kRectangle);
    return mark.top_left + 0.5 * mark.size;
  }

  inline cv::Rect DataToRoi(const TrackerData &tracker_data, const Point2 exp,
                            const double search_factor) const override {
    const auto f = search_factor;
    const auto &data = static_cast<const CamshiftData &>(tracker_data);

    cv::Size new_size(data.size.width * f, data.size.height * f);
    return cv::Rect(exp - 0.5 * Point2(data.size), new_size);
  }

 private:
  CamshiftData data_;
  
  cv::Mat PreprocessImage(const cv::Mat &data) const;

};

} // namespace dove_eye

#endif // DOVE_EYE_CAMSHIFT_TRACKER_H_
