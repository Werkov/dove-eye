#ifndef DOVE_EYE_VIDEO_PROVIDER_H_
#define DOVE_EYE_VIDEO_PROVIDER_H_

#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

#include "dove_eye/frame_iterator.h"

namespace dove_eye {

class VideoProvider {
 public:
  virtual std::string Id() const = 0;

  virtual FrameIterator begin() = 0;
  virtual FrameIterator end() = 0;
};

} // namespace dove_eye

#endif // DOVE_EYE_VIDEO_PROVIDER_H_

