#include "frameset_viewer.h"

#include <QPainter>
#include <QSizePolicy>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;

namespace gui {

void FramesetViewer::SetArity(const dove_eye::CameraIndex arity) {
  arity_ = arity;

  if (converter_ && converter_->Arity() != arity_) {
    converter_ = nullptr;
  }

  for (auto viewer : viewers_) {
    delete viewer;
  }
  viewers_.resize(arity_);

  auto new_layout = new QVBoxLayout();

  for (CameraIndex cam = 0; cam < arity_; ++cam) {
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
  assert(converter);
  assert(arity_ == converter->Arity());

  converter_ = converter;
  for (CameraIndex cam = 0; cam < arity_; ++cam) {
    viewers_[cam]->SetConverter(converter_, cam);
  }
}

void FramesetViewer::SetImageset(
    const FramesetConverter::ImageList &image_list) {
  assert(arity_ == image_list.size());

  for (CameraIndex cam = 0; cam < arity_; ++cam) {
    /* Display only images from valid frames. */
    if (image_list[cam].byteCount()) {
      viewers_[cam]->SetImage(image_list[cam]);
    }
  }
}

void FramesetViewer::SetPositset(const dove_eye::Positset positset) {
	assert(arity_ == positset.Arity());
  
  for (CameraIndex cam = 0; cam < arity_; ++cam) {
    if (positset.IsValid(cam)) {
      viewers_[cam]->SetPosit(positset[cam]);
    } else {
      viewers_[cam]->UnsetPosit();
    }
  }
}

} // namespace gui

