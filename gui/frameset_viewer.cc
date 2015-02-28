#include "frameset_viewer.h"

#include <QPainter>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;

namespace gui {

FramesetViewer *FramesetViewer::createWithLayout(
    const dove_eye::CameraIndex width,
    QLayout *layout,
    QWidget *parent) {
  auto result = new FramesetViewer(width, parent);

  for (CameraIndex cam = 0; cam < width; ++cam) {
    auto viewer = new FrameViewer();
    result->viewers_[cam] = viewer;
    layout->addWidget(viewer);
  }

  result->setLayout(layout);

  return result;
}

void FramesetViewer::SetImageset(
    const FramesetConverter::ImageList &image_list) {
  assert(width_ == image_list.size());

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    viewers_[cam]->SetImage(image_list[cam]);
  }
}

} // namespace gui

