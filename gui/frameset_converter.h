#ifndef GUI_FRAMESET_CONVERTER_H_
#define GUI_FRAMESET_CONVERTER_H_

#include <QBasicTimer>
#include <QImage>
#include <QObject>
#include <QVector>

#include "dove_eye/frameset.h"
#include "dove_eye/types.h"

namespace gui {

/*
 * \see http://stackoverflow.com/a/21253353/1351874
 */

class FramesetConverter : public QObject {
  Q_OBJECT

 public:
  typedef QVector<QImage> ImageList;

  explicit FramesetConverter(const dove_eye::CameraIndex width,
                             QObject *parent = nullptr)
      : QObject(parent),
        frameset_(width) {
  }

 signals:
  void ImagesetReady(const ImageList &);

 public slots:
  void ProcessFrameset(const dove_eye::Frameset &frameset);

 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  QBasicTimer timer_;
  dove_eye::Frameset frameset_;
  bool allow_drop_ = true;

  void ProcessFramesetInternal(dove_eye::Frameset frameset);

  void Enqueue(const dove_eye::Frameset &frameset);
};

} // namespace gui

#endif // GUI_FRAMESET_CONVERTER_H_
