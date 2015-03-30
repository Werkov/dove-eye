#ifndef DOVE_EY_CAMERA_VIDEO_PROVIDER_H_
#define DOVE_EY_CAMERA_VIDEO_PROVIDER_H_

#include <string>

#include "dove_eye/video_provider.h"

namespace dove_eye {

class CameraVideoProvider : public VideoProvider {
 public:
  explicit CameraVideoProvider(const int device);

  inline std::string Id() const {
    return id_;
  }

  FrameIterator begin() override;

  FrameIterator end() override;

 private:
  const int device_;
  std::string id_;

};

} // namespace dove_eye

#endif // DOVE_EY_CAMERA_VIDEO_PROVIDER_H_

