#include "dove_eye/file_video_provider.h"

#include <chrono>
#include <opencv2/opencv.hpp>

using cv::VideoCapture;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

namespace dove_eye {

FileVideoIterator::FileVideoIterator() : valid_(false) {

}

FileVideoIterator::FileVideoIterator(std::string filename) :
 videoCapture_(new VideoCapture(filename)) {
  valid_ = videoCapture_->isOpened();
  frameNo_ = 0;
  if (valid_) {
    framePeriod_ = 1 / videoCapture_->get(CV_CAP_PROP_FPS);
    operator++(); // load first frame
  }
  
}

FileVideoIterator& FileVideoIterator::operator++() {
  valid_ = videoCapture_->grab();
  valid_ = valid_ && videoCapture_->retrieve(frame_.data);
  frame_.timestamp = frameNo_ * framePeriod_;
  ++frameNo_;
}

Frame FileVideoIterator::operator*() const {
  return frame_;
}

bool FileVideoIterator::operator==(const FileVideoIterator& rhs) {
  return !valid_ && !rhs.valid_;
}

FileVideoProvider::FileVideoProvider(std::string filename) :
 filename_(filename),
 end_() {

}

FileVideoIterator FileVideoProvider::begin() {
  return FileVideoIterator(filename_);
}









}
