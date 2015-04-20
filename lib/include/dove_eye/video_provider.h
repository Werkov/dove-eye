#ifndef DOVE_EYE_VIDEO_PROVIDER_H_
#define DOVE_EYE_VIDEO_PROVIDER_H_

#include <atomic>
#include <memory>
#include <string>

#include <opencv2/opencv.hpp>

#include "dove_eye/calibration_data.h"
#include "dove_eye/frame_iterator.h"

namespace dove_eye {

class VideoProvider {
 public:
  VideoProvider()
      : camera_parameters_(nullptr),
        undistort_(false) {
  }

  virtual ~VideoProvider() {}

  virtual std::string Id() const = 0;

  virtual FrameIterator begin() = 0;
  virtual FrameIterator end() = 0;

  inline bool undistort() const {
    return undistort_;
  }

  /**
   * Setting undistort mode with camera parameters is thread safe
   */
  inline void undistort(const bool value) {
    undistort_ = value;
  }

  /**
   * Setting undistort mode with camera parameters is thread safe
   */
  inline const CameraParameters *camera_parameters() const {
    return camera_parameters_;
  }

  inline void camera_parameters(const CameraParameters *value) {
    camera_parameters_ = const_cast<CameraParameters *>(value);
  }

  /** Every frame of provider should be passed (by iterator) to this function
   *
   * @note Currently it's not virtual as there is no reason to extend here
   *       (premature optimization)
   */
  void PreprocessFrame(Frame *frame) const;

 private:
  std::atomic<CameraParameters *> camera_parameters_;
  std::atomic<bool> undistort_;
};

} // namespace dove_eye

#endif // DOVE_EYE_VIDEO_PROVIDER_H_

