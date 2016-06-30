#include "dove_eye/tld_tracker.h"

#include <tld/TLD.h>

namespace dove_eye {
  bool TldTracker::InitTrackerData(const cv::Mat &data) {
    tld = new tld::TLD();
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

    tld->detectorCascade->imgWidth = data_.grey.cols;
    tld->detectorCascade->imgHeight = data_.grey.rows;
    tld->detectorCascade->imgWidthStep = data_.grey.step;

    cv::Rect bb(mark.top_left, mark.size);
    tld->selectObject(data_.grey, &bb);

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
    if (!data_.reuseFrameOnce) {
      cvtColor(frame.data, data_.grey, CV_BGR2GRAY);
    } else {
      data_.reuseFrameOnce = false;
    }

    if (!data_.skipProcessingOnce) {
      tld->processImage(frame.data);
    } else {
      data_.skipProcessingOnce = false;
    }

    if (tld->currConf < data_.threshold) {
      return false;
    }

    *result = (tld->currBB->tl() + tld->currBB->br()) / 2;
    return true;
  }
} // namespace dove_eye
