#include "frame_viewer.h"

#include <QPainter>

#include "dove_eye/logging.h"
#include "gui_mark.h"

using dove_eye::CameraIndex;


namespace gui {


void FrameViewer::SetImage(const QImage &image) {
  if (!image_.isNull()) {
    DEBUG("Viewer dropped a frame!");
  }
  image_ = image;
  update();
}

void FrameViewer::SetConverter(FramesetConverter *converter,
                               const CameraIndex cam) {
  converter_ = converter;
  cam_ = cam;
}

void FrameViewer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawImage(0, 0, image_);
  image_ = QImage();
}

void FrameViewer::resizeEvent(QResizeEvent *event) {
  if (converter_) {
    converter_->SetFrameSize(cam_, event->size());
  }
}

void FrameViewer::mouseReleaseEvent(QMouseEvent *event) {
  if (converter_) {
    GuiMark mark;
    mark.pos = event->pos();

    if (event->modifiers() & Qt::ControlModifier) {
      mark.flags |= GuiMark::kCtrl;
    }
    if (event->modifiers() & Qt::ShiftModifier) {
      mark.flags |= GuiMark::kShift;
    }

    converter_->PropagateMark(cam_, mark);
  }
}

} // namespace gui

