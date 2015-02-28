#ifndef GUI_FRAMESET_VIEWER_H_
#define GUI_FRAMESET_VIEWER_H_

#include <QLayout>
#include <QVector>
#include <QWidget>

#include "dove_eye/types.h"
#include "frame_viewer.h"
#include "frameset_converter.h"

class QLayout;

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class FramesetViewer : public QWidget {
  Q_OBJECT

 public:
  explicit FramesetViewer(const dove_eye::CameraIndex width,
                          QWidget *parent = nullptr)
      : QWidget(parent),
        width_(width),
        viewers_(width) {
  }

  static FramesetViewer *createWithLayout(const dove_eye::CameraIndex width,
                                          QLayout *layout,
                                          QWidget *parent);

 public slots:
  void SetImageset(const FramesetConverter::ImageList &image_list);

 private:
  const dove_eye::CameraIndex width_;
  QVector<FrameViewer *> viewers_;
};

} // namespace gui

#endif // GUI_FRAMESET_VIEWER_H_
