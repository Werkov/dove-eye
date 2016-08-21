#ifndef DOVE_EYE_TLD_TRACKER_H_
#define DOVE_EYE_TLD_TRACKER_H_

#include <memory>

#include "dove_eye/cv_kalman_filter.h"
#include "dove_eye/inner_tracker.h"

namespace tld {
/* Forward */
class TLD;
} // namespace tld

namespace dove_eye {

class TldTracker : public InnerTracker {
 public:
  struct TldData : public TrackerData {
    bool reuseFrameOnce;
    bool skipProcessingOnce;
    cv::Mat grey;

    TldData() {
      reuseFrameOnce = true;
      skipProcessingOnce = true;
    }
  };

  explicit TldTracker(const Parameters &parameters);

  TldTracker(const TldTracker &other);

  ~TldTracker() override;

  inline const TrackerData &tracker_data() const override {
    return data_;
  }

  inline TrackerData &tracker_data() override {
    return data_;
  }

  bool InitializeTracking(const Frame &frame, const Mark mark,
    Posit *result) override;

  bool InitializeTracking(
    const Frame &frame,
    const Epiline epiline,
    const TrackerData &tracker_data,
    Posit *result) override;

  /** Track the given frame */
  bool Track(const Frame &frame, Posit *result) override;

  /** Global reinitialization */
  bool ReinitializeTracking(const Frame &frame, Posit *result) override;

  /** Reinitialization from epiline */
  inline bool ReinitializeTracking(const Frame &frame,
    const Epiline epiline,
    Posit *result) {
    return ReinitializeTracking(frame, result);
  }

  /** Reinitalization from projection point guess */
  inline bool ReinitializeTracking(const Frame &frame,
    const Point2 guess,
    Posit *result) {
    return ReinitializeTracking(frame, result);
  }

  InnerTracker *Clone() const override {
    assert(!initialized());

    return new TldTracker(*this);
  }

  inline InnerTracker::Mark::Type PreferredMarkType() const {
    return InnerTracker::Mark::kRectangle;
  }

 private:
  bool initialized_;
  std::unique_ptr<tld::TLD> tld_;

  TldData data_;

  bool InitTrackerData(const cv::Mat &data);

  inline Posit MarkToPosit(const Mark &mark) const {
    assert(mark.type == Mark::kRectangle);
    return mark.top_left + 0.5 * mark.size;
  }

  inline bool initialized() const  {
    return initialized_;
  }

  inline void initialized(const bool value) {
    initialized_ = value;
  }

  bool RunTracking(const Frame &frame, Posit *result);
};

} // namespace dove_eye
#endif // DOVE_EYE_TLD_TRACKER_H_
