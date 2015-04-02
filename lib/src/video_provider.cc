#include "dove_eye/video_provider.h"

#include <cassert>

#include <opencv2/opencv.hpp>

namespace dove_eye {

void VideoProvider::PreprocessFrame(Frame *frame) const {
  if (!undistort()) {
    return;
  }

  assert(camera_parameters());
  cv::Mat undistored;
  cv::undistort(frame->data, undistored,
                camera_parameters()->camera_matrix,
                camera_parameters()->distortion_coefficients);
  frame->data = undistored;
}

} // end namespace dove_eye
