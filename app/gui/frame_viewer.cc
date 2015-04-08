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
  if (pressed_) {
    DrawMark(painter);
  }
  image_ = QImage();
}

void FrameViewer::resizeEvent(QResizeEvent *event) {
  if (converter_) {
    converter_->SetFrameSize(cam_, event->size());
  }
}

void FrameViewer::mousePressEvent(QMouseEvent *event) {
  pressed_ = true;
  InitMark(event);
}

void FrameViewer::mouseMoveEvent(QMouseEvent *event) {
  if (!pressed_) {
    return;
  }
  UpdateMark(event);
}

void FrameViewer::mouseReleaseEvent(QMouseEvent *event) {
  pressed_ = false;
  UpdateMark(event);

  if (converter_) {
    converter_->PropagateMark(cam_, mark_);
  }
}

void FrameViewer::InitMark(QMouseEvent *event) {
  mark_.press_pos = event->pos();
  mark_.release_pos = event->pos();
}

void FrameViewer::UpdateMark(QMouseEvent *event) {
  mark_.release_pos = event->pos();

  mark_.flags = GuiMark::kNoFlags;
  if (event->modifiers() & Qt::ControlModifier) {
    mark_.flags |= GuiMark::kCtrl;
  }
  if (event->modifiers() & Qt::ShiftModifier) {
    mark_.flags |= GuiMark::kShift;
  }
}

void FrameViewer::DrawMark(QPainter &painter) {
  auto brush = painter.brush();

  QColor color(32, 32, 32, 128);
  painter.setBrush(color);

  painter.drawRect(QRect(mark_.TopLeft(), mark_.Size()));

  painter.setBrush(brush);
}
} // namespace gui

