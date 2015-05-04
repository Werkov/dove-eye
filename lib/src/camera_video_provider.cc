#include "dove_eye/camera_video_provider.h"

#include <sstream>

#include "dove_eye/cv_frame_iterator.h"
#include "dove_eye/frame_iterator/clock_policy.h"
#include "dove_eye/frame_iterator/nonblocking_policy.h"

namespace dove_eye {

/* Provider */
CameraVideoProvider::CameraVideoProvider(const int device)
    : VideoProvider(),
      device_(device) {
  std::stringstream ss;
  ss << "Device " << device;
  id_ = ss.str();
}

FrameIterator CameraVideoProvider::begin() {
  typedef CvFrameIterator<frame_iterator::ClockPolicy,
                          frame_iterator::NonblockingPolicy> CvIterator;

  auto cv_iterator = new CvIterator(device_);

  /* Apply settings */
  if (resolution_.width > 0 && resolution_.height > 0) {
    auto &capture = cv_iterator->CvVideoCapture();
    capture.set(CV_CAP_PROP_FRAME_WIDTH, resolution().width);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, resolution().height);
  }

  return FrameIterator(this, cv_iterator);
}

FrameIterator CameraVideoProvider::end() {
  return FrameIterator(this);
}

CameraVideoProvider::ResolutionVector
CameraVideoProvider::AvailableResolutions() const {
  /*
   * OpenCV API doesn't allow access to available resolutions, so just use some
   * most common values.
   */
  return {
    Resolution(160, 120),
    Resolution(320, 240),
    Resolution(640, 480),
    Resolution(640, 360),
    Resolution(1280, 720)
  };
}

} // namespace dove_eye
