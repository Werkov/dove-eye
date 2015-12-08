#include "dove_eye/file_video_provider.h"

#include <cassert>
#include <thread>

#include "dove_eye/cv_frame_iterator.h"
#include "dove_eye/frame_iterator/nonblocking_policy.h"
#include "dove_eye/frame_iterator/fps_policy.h"

namespace dove_eye {

/* Provider */
FileVideoProvider::FileVideoProvider(const std::string &filename)
    : VideoProvider(),
      filename_(filename) {
}

FrameIterator FileVideoProvider::begin() {
  typedef CvFrameIterator<frame_iterator::FpsPolicy,
                          frame_iterator::NonblockingPolicy> CvIterator;

  return FrameIterator(this, new CvIterator(filename_));
}

FrameIterator FileVideoProvider::end() {
  return FrameIterator(this);
}

} // namespace dove_eye
