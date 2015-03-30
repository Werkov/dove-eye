#include "dove_eye/camera_video_provider.h"

#include <sstream>

#include "dove_eye/cv_frame_iterator.h"
#include "dove_eye/frame_iterator/clock_policy.h"
#include "dove_eye/frame_iterator/nonblocking_policy.h"

namespace dove_eye {

/* Provider */
CameraVideoProvider::CameraVideoProvider(const int device)
    : device_(device) {
  std::stringstream ss;
  ss << "Device " << device;
  id_ = ss.str();
}

FrameIterator CameraVideoProvider::begin() {
  typedef CvFrameIterator<frame_iterator::ClockPolicy,
                          frame_iterator::NonblockingPolicy> CvIterator;

  return FrameIterator(new CvIterator(device_));
}

FrameIterator CameraVideoProvider::end() {
  return FrameIterator();
}

} // namespace dove_eye
