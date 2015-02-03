#ifndef DOVE_EYE_VIDEO_PROVIDER_H_
#define	DOVE_EYE_VIDEO_PROVIDER_H_

#include <chrono>
#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

#include <dove_eye/frame.h>

#include <iostream>

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
  // We share VideoCapture object among copies.
  std::shared_ptr<cv::VideoCapture> videoCapture_;
  bool valid_;
  Frame frame_;
  int frameNo_;
  double framePeriod_;
};

class FileVideoProvider {
 public:
  // Creates a new iterator, all copies share the state.
  FileVideoIterator begin();

  inline FileVideoIterator &end() {
    return end_;
  }

  FileVideoProvider(std::string filename);

 private:
  std::string filename_;
  FileVideoIterator end_;
};

} // namespace dove_eye

#endif	/* DOVE_EYE_VIDEO_PROVIDER_H_ */

