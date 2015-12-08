#ifndef GUI_FRAME_VIEWER_H_
#define GUI_FRAME_VIEWER_H_

#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QWidget>

#include "dove_eye/types.h"
#include "frameset_converter.h"

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class FrameViewer : public QWidget {
  Q_OBJECT

 public:
  explicit FrameViewer(QWidget *parent = nullptr)
      : QWidget(parent),
        undrawn_image_(false),
        converter_(nullptr),
        pressed_(false) {
  }

  void SetConverter(FramesetConverter *converter, dove_eye::CameraIndex cam);

 public slots:
  void SetImage(const QImage &image);

 protected:
  void paintEvent(QPaintEvent *event) override;

  void resizeEvent(QResizeEvent *event) override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

 private:
  QImage image_;
  bool undrawn_image_;
  FramesetConverter *converter_;
  dove_eye::CameraIndex cam_;

  GuiMark mark_;
  bool pressed_;

  void InitMark(QMouseEvent *event);

  void UpdateMark(QMouseEvent *event);

  void DrawMark(QPainter &painter);
};

} // namespace gui

#endif // GUI_FRAME_VIEWER_H_
