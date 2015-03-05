#include "frame_viewer.h"

#include <QPainter>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;


namespace gui {


void FrameViewer::SetImage(const QImage &image) {
  if (!image_.isNull()) {
    DEBUG("Viewer dropped a frame!\n");
  }
  image_ = image;
  update();
}

void FrameViewer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawImage(0, 0, image_);
  image_ = QImage();
}

void FrameViewer::resizeEvent(QResizeEvent *event) {
  DEBUG("frame resized to: %i, %i\n", event->size().width(), event->size().height());
}


} // namespace gui

