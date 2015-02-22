#include "dove_eye/file_video_provider.h"

namespace dove_eye {

/* Provider */
FileVideoProvider::FileVideoProvider(const std::string &filename) :
 filename_(filename) {

}

FrameIterator FileVideoProvider::begin() {
  return FrameIterator(new FileVideoProvider::Iterator(*this));
}

FrameIterator FileVideoProvider::end() {
  return FrameIterator();
}

/* Iterator */

FileVideoProvider::Iterator::Iterator(const FileVideoProvider &provider) :
 videoCapture_(new cv::VideoCapture(provider.filename_)) {
  valid_ = videoCapture_->isOpened();
  frameNo_ = 0;
  if (valid_) {
    framePeriod_ = 1 / videoCapture_->get(CV_CAP_PROP_FPS);
    /* NOTE: This cannot be used for camera capture (would block) */
    MoveNext(); // load first frame
  }
  
}

Frame FileVideoProvider::Iterator::GetFrame() const {
  return frame_;
}

void FileVideoProvider::Iterator::MoveNext() {
  valid_ = videoCapture_->grab();
  valid_ = valid_ && videoCapture_->retrieve(frame_.data);
  frame_.timestamp = frameNo_ * framePeriod_;
  ++frameNo_;
}

}
