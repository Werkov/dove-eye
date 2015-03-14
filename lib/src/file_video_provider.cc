#include "dove_eye/file_video_provider.h"

#include <cassert>
#include <thread>

namespace dove_eye {

/* Provider */
FileVideoProvider::FileVideoProvider(const std::string &filename,
                                     const bool blocking)
    : filename_(filename),
      blocking_(blocking) {
}

FrameIterator FileVideoProvider::begin() {
  return FrameIterator(new FileVideoProvider::Iterator(*this));
}

FrameIterator FileVideoProvider::end() {
  return FrameIterator();
}

/* Iterator -- static fields */

std::mutex FileVideoProvider::Iterator::capture_lifecycle_mtx_;

FileVideoProvider::Iterator::CaptureDeleter
    FileVideoProvider::Iterator::capture_deleter_;

/* Iterator -- methods */

void FileVideoProvider::Iterator::CaptureDeleter::operator()(
    cv::VideoCapture *to_delete) const {
  CaptureLock lock(capture_lifecycle_mtx_);
  delete to_delete;
}

FileVideoProvider::Iterator::Iterator(const FileVideoProvider &provider)
    : provider_(provider) {
  /*
   * Creation and disposal of cv::VideoCapture has to be synchronized,
   * as it is not internally thread safe.
   */
  {
    CaptureLock lock(capture_lifecycle_mtx_);
    auto capture = new cv::VideoCapture(provider_.filename_);

    video_capture_ = CvCapturePtr(capture, Iterator::capture_deleter_);
  }

  valid_ = video_capture_->isOpened();
  frame_no_ = 0;
  if (valid_) {
    assert(video_capture_->get(CV_CAP_PROP_FPS) > 0);
    frame_period_ = 1 / video_capture_->get(CV_CAP_PROP_FPS);
    /* NOTE: This cannot be used for camera capture (would block) */
    MoveNext(); // load first frame
  }
}

Frame FileVideoProvider::Iterator::GetFrame() const {
  return frame_.Clone();
}

void FileVideoProvider::Iterator::MoveNext() {
  valid_ = video_capture_->grab();
  valid_ = valid_ && video_capture_->retrieve(frame_.data);
  frame_.timestamp = frame_no_ * frame_period_;
  ++frame_no_;
  if (provider_.blocking_) {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(frame_period_ * 1000)));
  }
}

} // namespace dove_eye
