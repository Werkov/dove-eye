#ifndef DOVE_EYE_CAMERA_VIDEO_PROVIDER_H_
#define DOVE_EYE_CAMERA_VIDEO_PROVIDER_H_

#include <string>
#include <vector>

#include "dove_eye/video_provider.h"

namespace dove_eye {

class CameraVideoProvider : public VideoProvider {
 public:
  struct Resolution {
    size_t width;
    size_t height;

    explicit Resolution(size_t w = 0, size_t h = 0) : width(w), height(h) {
    }
  };

  typedef std::vector<Resolution> ResolutionVector;

  explicit CameraVideoProvider(const int device);

  inline std::string Id() const {
    return id_;
  }

  FrameIterator begin() override;

  FrameIterator end() override;

  ResolutionVector AvailableResolutions() const;

  inline Resolution resolution() const {
    return resolution_;
  }

  /** Set desired resolution of provider's output to default value
   */
  inline void SetDefaultResolution() {
    resolution_ = Resolution();
  }

  /** Set desired resolution of provider's output
   */
  inline void resolution(const size_t width, const size_t height) {
    resolution_ = Resolution(width, height);
  }

 private:
  const int device_;
  std::string id_;
  /** 0x0 means default (unmodified) size */
  Resolution resolution_;
};

} // namespace dove_eye

#endif // DOVE_EYE_CAMERA_VIDEO_PROVIDER_H_

