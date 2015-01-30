#ifndef DOVE_EYE_VIDEO_PROVIDER_H_
#define	DOVE_EYE_VIDEO_PROVIDER_H_

#include <chrono>
#include <opencv2/opencv.hpp>
#include <string>

#include <dove_eye/frame.h>

namespace dove_eye {

class FileVideoIterator {
 public:
  FileVideoIterator();

  FileVideoIterator(std::string filename);

  Frame operator*() const;

  FileVideoIterator &operator++();

  bool operator==(const FileVideoIterator &rhs);

  inline bool operator!=(const FileVideoIterator &rhs) {
    return !operator==(rhs);
  }

 private:
  //TODO can videoCapture be copied?
  cv::VideoCapture videoCapture_;
  bool valid_;
  Frame frame_;
  int frameNo_;
};

class FileVideoProvider {
 public:

  inline FileVideoIterator &begin() {
    return begin_;
  }

  inline FileVideoIterator &end() {
    return end_;
  }

  FileVideoProvider(std::string filename);

 private:
  FileVideoIterator begin_;
  FileVideoIterator end_;
};

} // namespace dove_eye

#endif	/* DOVE_EYE_VIDEO_PROVIDER_H_ */

