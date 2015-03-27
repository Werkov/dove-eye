#ifndef GUI_FRAMESET_CONVERTER_H_
#define GUI_FRAMESET_CONVERTER_H_

#include <QBasicTimer>
#include <QImage>
#include <QObject>
#include <QVector>

#include "dove_eye/frameset.h"
#include "dove_eye/types.h"
#include "gui_mark.h"

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 *
 * Converts frameset to vector of QImage. If a frame is not valid,
 * default empty QImage instance is returned.
 */

class FramesetConverter : public QObject {
  Q_OBJECT

 public:
  typedef QVector<QImage> ImageList;

  explicit FramesetConverter(const dove_eye::CameraIndex arity,
                             QObject *parent = nullptr)
      : QObject(parent),
        frameset_(arity),
        frame_sizes_(arity),
        viewer_sizes_(arity) {
  }

  inline dove_eye::CameraIndex Arity() const {
    return frameset_.Arity();
  }

  void SetFrameSize(const dove_eye::CameraIndex cam, const QSize size);

  void PropagateMark(const dove_eye::CameraIndex cam,
                     const GuiMark mark);

 signals:
  void ImagesetReady(const ImageList &);

  void MarkCreated(const dove_eye::CameraIndex cam,
                   const GuiMark mark);
  //void MarkCreated(const int cam,
  //                 const GuiMark mark);

 public slots:
  void ProcessFrameset(const dove_eye::Frameset &frameset);

 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  QBasicTimer timer_;
  dove_eye::Frameset frameset_;
  bool allow_drop_ = true;
  QVector<QSize> frame_sizes_;
  QVector<QSize> viewer_sizes_;

  void ProcessFramesetInternal(dove_eye::Frameset frameset);

  void Enqueue(const dove_eye::Frameset &frameset);
};

} // namespace gui

Q_DECLARE_METATYPE(gui::FramesetConverter::ImageList)

#endif // GUI_FRAMESET_CONVERTER_H_
