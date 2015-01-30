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
 videoCapture_(filename) {
  valid_ = videoCapture_.isOpened();
  frameNo_ = 0;
}

FileVideoIterator& FileVideoIterator::operator++() {
  valid_ = videoCapture_.grab();
  // TODO get framerate from codec information
  frame_.timestamp = frameNo_ * 33; // 33 ms per frame
  valid_ = valid_ && videoCapture_.retrieve(frame_.data);
}

Frame FileVideoIterator::operator*() const {
  return frame_;
}

bool FileVideoIterator::operator==(const FileVideoIterator& rhs) {
  return !valid_ && !rhs.valid_;
}

FileVideoProvider::FileVideoProvider(std::string filename) :
 begin_(filename),
 end_() {

}








}
