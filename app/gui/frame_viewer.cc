#include "frame_viewer.h"

#include <QPainter>

#include "dove_eye/logging.h"
#include "gui_mark.h"

using dove_eye::CameraIndex;


namespace gui {


void FrameViewer::SetImage(const QImage &image) {
  if (undrawn_image_) {
    DEBUG("Viewer dropped a frame!");
  }
  image_ = image;
  undrawn_image_ = true;
  update();
}

void FrameViewer::SetPosit(const dove_eye::Posit posit) {
  posit_ = posit;
  has_posit_ = true;
  update();
}

void FrameViewer::UnsetPosit() {
  has_posit_ = false;
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
  undrawn_image_ = false;

  if (has_posit_) {
    DrawPosit(painter);
    /* has_posit_ = true, is kept (may be still shown) */
  }

  if (pressed_) {
    DrawMark(painter);
  }
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
  /* This results in paintEvent() being queued. */
  update();
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

  update();
}

void FrameViewer::DrawMark(QPainter &painter) {
  auto brush = painter.brush();

  QColor color(51, 102, 255, 64);
  painter.setBrush(color);

  painter.drawRect(QRect(mark_.TopLeft(), mark_.Size()));

  painter.setBrush(brush);
}

void FrameViewer::DrawPosit(QPainter &painter) {
  /* This method could be later extracted to a separate class (when Posit will
   * become more complex. */
  const int radius = 8;
  const int width = 2;
  auto pen = painter.pen();

  QColor color(51, 102, 255, 128);
  QPen draw_pen(color);
  draw_pen.setWidth(width);

  int rectWidth = mark_.Size().width();
  int rectHeight = mark_.Size().height();

  DEBUG("%s: %f,%f", __func__, posit_.x, posit_.y);

  painter.setPen(draw_pen);
  painter.drawRect(posit_.x - rectWidth / 2, posit_.y, rectWidth, rectHeight);
  //painter.drawEllipse(posit_.x - radius, posit_.y - radius, 2*radius, 2*radius);

  painter.setPen(pen);
}

} // namespace gui

