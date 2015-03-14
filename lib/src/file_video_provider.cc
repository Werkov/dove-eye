#include "dove_eye/file_video_provider.h"

namespace dove_eye {

/* Provider */
FileVideoProvider::FileVideoProvider(const std::string &filename)
    : filename_(filename) {
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

FileVideoProvider::Iterator::Iterator(const FileVideoProvider &provider) {
  /*
   * Creation and disposal of cv::VideoCapture has to be synchronized,
   * as it is not internally thread safe.
   */
  {
    CaptureLock lock(capture_lifecycle_mtx_);
    auto capture = new cv::VideoCapture(provider.filename_);

    video_capture_ = CvCapturePtr(capture, Iterator::capture_deleter_);
  }

  valid_ = video_capture_->isOpened();
  frame_no_ = 0;
  if (valid_) {
    frame_period_ = 1 / video_capture_->get(CV_CAP_PROP_FPS);
    /* NOTE: This cannot be used for camera capture (would block) */
    MoveNext(); // load first frame
  }
}

Frame FileVideoProvider::Iterator::GetFrame() const {
  return frame_;
}

void FileVideoProvider::Iterator::MoveNext() {
  valid_ = video_capture_->grab();
  valid_ = valid_ && video_capture_->retrieve(frame_.data);
  frame_.timestamp = frame_no_ * frame_period_;
  ++frame_no_;
}

} // namespace dove_eye
