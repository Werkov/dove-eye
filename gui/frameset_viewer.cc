#include "frameset_viewer.h"

#include <QPainter>
#include <QSizePolicy>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;

namespace gui {

void FramesetViewer::SetWidth(const dove_eye::CameraIndex width) {
  width_ = width;

  for (auto viewer : viewers_) {
    delete viewer;
  }
  viewers_.resize(width_);

  auto layout = new QVBoxLayout();

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    auto viewer = new FrameViewer(this);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    viewer->setSizePolicy(sizePolicy);

    layout->addWidget(viewer);
    this->viewers_[cam] = viewer;
  }

  delete this->layout();
  this->setLayout(layout);
}

void FramesetViewer::SetImageset(
    const FramesetConverter::ImageList &image_list) {
  assert(width_ == image_list.size());

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    viewers_[cam]->SetImage(image_list[cam]);
  }
}

void FramesetViewer::resizeEvent(QResizeEvent *event) {
  DEBUG("resized to: %i, %i\n", event->size().width(), event->size().height());
}

} // namespace gui

