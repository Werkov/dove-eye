#include "frameset_viewer.h"

#include <QPainter>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;

namespace gui {

void FramesetViewer::SetWidth(const dove_eye::CameraIndex width) {
  width_ = width;

  for (auto viewer : viewers_) {
    delete viewer;
  }
  viewers_.resize(width_);

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    auto viewer = new FrameViewer(this);
    this->viewers_[cam] = viewer;
  }
}

void FramesetViewer::SetImageset(
    const FramesetConverter::ImageList &image_list) {
  assert(width_ == image_list.size());

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    viewers_[cam]->SetImage(image_list[cam]);
  }
}

} // namespace gui

