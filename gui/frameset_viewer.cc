#include "frameset_viewer.h"

#include <QPainter>
#include <QSizePolicy>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;

namespace gui {

void FramesetViewer::SetWidth(const dove_eye::CameraIndex width) {
  width_ = width;

  if (converter_ && converter_->width() != width_) {
    converter_ = nullptr;
  }

  for (auto viewer : viewers_) {
    delete viewer;
  }
  viewers_.resize(width_);

  auto new_layout = new QVBoxLayout();

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    auto viewer = new FrameViewer(this);

    if (converter_) {
      viewer->SetConverter(converter_, cam);
    }

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    viewer->setSizePolicy(sizePolicy);

    new_layout->addWidget(viewer);
    viewers_[cam] = viewer;
  }

  delete layout();
  setLayout(new_layout);
}

void FramesetViewer::SetConverter(FramesetConverter *converter) {
  assert(width_ == converter->width());

  converter_ = converter;
  for (CameraIndex cam = 0; cam < width_; ++cam) {
    viewers_[cam]->SetConverter(converter_, cam);
  }
}

void FramesetViewer::SetImageset(
    const FramesetConverter::ImageList &image_list) {
  assert(width_ == image_list.size());

  for (CameraIndex cam = 0; cam < width_; ++cam) {
    /* Display only images from valid frames. */
    if (image_list[cam].byteCount()) {
      viewers_[cam]->SetImage(image_list[cam]);
    }
  }
}

} // namespace gui

