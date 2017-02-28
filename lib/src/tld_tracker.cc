#include "dove_eye/tld_tracker.h"

#include <tld/TLD.h>

namespace dove_eye {
  TldTracker::TldTracker(const Parameters &parameters)
      : InnerTracker(parameters),
        initialized_(false) {
  }

  TldTracker::TldTracker(const TldTracker &other)
      : InnerTracker(other),
        initialized_(other.initialized_),
        tld_(nullptr),
        data_(other.data_) {
    /* We can copy unitialized object only, without allocated TLD object */
    assert(!other.initialized());
  }

  TldTracker::~TldTracker() {
  }

  bool TldTracker::InitTrackerData(const cv::Mat &data) {
    tld_.reset(new tld::TLD());
    tracker_data();
    data_.grey = cv::Mat(data.rows, data.cols, CV_8UC1);

    return true;
  }

  bool TldTracker::InitializeTracking(const Frame &frame, const Mark mark,
    Posit *result) {

    if (!InitTrackerData(frame.data)) {
      return false;
    }

    initialized(true);

    cvtColor(frame.data, data_.grey, CV_BGR2GRAY);

    tld_->detectorCascade->imgWidth = data_.grey.cols;
    tld_->detectorCascade->imgHeight = data_.grey.rows;
    tld_->detectorCascade->imgWidthStep = data_.grey.step;

    cv::Rect bb(mark.top_left, mark.top_left + mark.size);
    tld_->selectObject(data_.grey, &bb);

    *result = MarkToPosit(mark);

    return true;
  }

  bool TldTracker::InitializeTracking(
    const Frame &frame,
    const Epiline epiline,
    const TrackerData &tracker_data,
    Posit *result) {
    // TODO implement initialization from epiline
    return false;
  }

  bool TldTracker::Track(const Frame &frame, Posit *result) {
    assert(initialized());

    return RunTracking(frame, result);
  }

  bool TldTracker::ReinitializeTracking(const Frame &frame, Posit *result) {
    assert(initialized());

    return RunTracking(frame, result);
  }

  bool TldTracker::RunTracking(const Frame &frame, Posit *result) {
    const auto thr = parameters().Get(Parameters::SEARCH_THRESHOLD);
    if (!data_.reuseFrameOnce) {
      cvtColor(frame.data, data_.grey, CV_BGR2GRAY);
    } else {
      data_.reuseFrameOnce = false;
    }

    if (!data_.skipProcessingOnce) {
      tld_->processImage(frame.data);
    } else {
      data_.skipProcessingOnce = false;
    }

    if (tld_->currConf < thr) {
      return false;
    }

    *result = 0.5 * (tld_->currBB->tl() + tld_->currBB->br());
    return true;
  }

} // namespace dove_eye
