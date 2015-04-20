#ifndef DOVE_EYE_HISTOGRAM_TRACKER_H_
#define DOVE_EYE_HISTOGRAM_TRACKER_H_

#include <vector>

#include <opencv2/opencv.hpp>

#include "dove_eye/searching_tracker.h"

namespace dove_eye {

class HistogramTracker : public SearchingTracker {
 public:
  struct HistogramData : public TrackerData {
    const int histogram_size = 16;
    /* Hue range is float because of calcBackProject API */
    float hrange[2];
    double srange[2];
    double vrange[2];

    cv::Mat histogram;
    /** Size of region from which histogram was taken */
    cv::Size size;

    HistogramData() {
        /* Because of MSVC bug with error C2536, use struct ctor. */
	hrange[0] = 0;
	hrange[1] = 180;
    }
  };

  explicit HistogramTracker(const Parameters &parameters)
      : SearchingTracker(parameters) {
  }

  inline const TrackerData &tracker_data() const override {
    return data_;
  }
 
  InnerTracker *Clone() const override {
    assert(!initialized());

    return new HistogramTracker(*this);
  }

 protected:
  bool InitTrackerData(const cv::Mat &data, const Mark &mark) override;

  bool Search(
      const cv::Mat &data,
      const TrackerData &tracker_data,
      const cv::Rect *roi,
      const cv::Mat *mask,
      const double threshold,
      Mark *result) const override;

  inline Posit MarkToPosit(const Mark &mark) const override {
    assert(mark.type == Mark::kRectangle);
    return mark.top_left + 0.5 * mark.size;
  }

  inline cv::Rect DataToRoi(const TrackerData &tracker_data, const Point2 exp,
                            const double search_factor) const override {
    const auto f = search_factor;
    const auto &data = static_cast<const HistogramData &>(tracker_data);

    cv::Size new_size(data.size.width * f, data.size.height * f);
    return cv::Rect(exp - 0.5 * Point2(new_size), new_size);
  }

 private:
  typedef std::vector<cv::Point> Contour;
  typedef std::vector<Contour> ContourVector;

  HistogramData data_;
  
  cv::Mat PreprocessImage(const cv::Mat &data,
                          const HistogramData &hist_data,
                          cv::Mat *mask) const;

  void ContoursToMark(const ContourVector &contours,
                        Mark *mark) const;
};

} // namespace dove_eye

#endif // DOVE_EYE_HISTOGRAM_TRACKER_H_
