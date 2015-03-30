#ifndef DOVE_EYE_CV_FRAME_ITERATOR_H_
#define DOVE_EYE_CV_FRAME_ITERATOR_H_

#include <memory>
#include <mutex>

#include <opencv2/opencv.hpp>

#include "dove_eye/cv_capture_lock.h"
#include "dove_eye/video_provider.h"

namespace dove_eye {

/**
 * CvFrameIterator uses global mutex dove_eye::cv_capture_mtx to serialize
 * access to OpenCV capture creation/disposal.
 */
template<typename TimestampPolicy, typename BlockingPolicy>
class CvFrameIterator : public FrameIteratorImpl {
 public:
  template<typename T>
  explicit CvFrameIterator(const T arg) {
    {
      CaptureLock lock(dove_eye::cv_capture_mtx);
      auto capture = new cv::VideoCapture(arg);

      video_capture_ = CvCapturePtr(capture, capture_deleter_);
    }

    valid_ = video_capture_->isOpened();
    if (valid_) {
      timestamp_policy_.Initialize(video_capture_.get());
      blocking_policy_.Initialize(video_capture_.get());
    }
  }

  Frame GetFrame() const override {
    /* Use separate data buffer (for various processings) */
    return frame_.Clone();
  }

  void MoveNext() override {
    valid_ = video_capture_->grab();
    valid_ = valid_ && video_capture_->retrieve(frame_.data);
    frame_.timestamp = timestamp_policy_.GetTimestamp();
    blocking_policy_.Wait();
  }

  bool IsValid() override {
    return valid_;
  }

 private:
  struct CaptureDeleter {
    void operator()(cv::VideoCapture *to_delete) const {
      CaptureLock lock(dove_eye::cv_capture_mtx);
      delete to_delete;
    }
  };

  CaptureDeleter capture_deleter_;

  typedef std::unique_ptr<cv::VideoCapture, decltype(capture_deleter_)>
      CvCapturePtr;
  typedef std::lock_guard<std::mutex> CaptureLock;

  // TODO restrict copy ctor
  //CvFrameIterator(const Iterator &);

  /* Unique ownership, cannot copy the iterator anyway. */
  CvCapturePtr video_capture_;

  bool valid_;
  Frame frame_;

  TimestampPolicy timestamp_policy_;
  BlockingPolicy blocking_policy_;
};

} // namespace dove_eye

#endif // DOVE_EYE_CV_FRAME_ITERATOR_H_

