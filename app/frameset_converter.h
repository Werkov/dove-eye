#ifndef FRAMESET_CONVERTER_H_
#define FRAMESET_CONVERTER_H_

#include <QBasicTimer>
#include <QImage>
#include <QObject>
#include <QVector>

#include "dove_eye/frameset.h"
#include "dove_eye/positset.h"
#include "dove_eye/types.h"
#include "gui/gui_mark.h"


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

  explicit FramesetConverter(const dove_eye::CameraIndex arity)
      : QObject(),
        has_frameset_(false),
        frameset_(arity),
        has_positset_(false),
        positset_(arity),
        frame_sizes_(arity),
        viewer_sizes_(arity) {
  }

  inline dove_eye::CameraIndex Arity() const {
    return frameset_.Arity();
  }

  void SetFrameSize(const dove_eye::CameraIndex cam, const QSize size);

  void PropagateMark(const dove_eye::CameraIndex cam,
                     const gui::GuiMark mark);

 signals:
  void ImagesetReady(const ImageList &);
  void PositsetReady(const dove_eye::Positset);

  void MarkCreated(const dove_eye::CameraIndex cam,
                   const gui::GuiMark mark);

 public slots:
  void ProcessFrameset(const dove_eye::Frameset &frameset);
  void ProcessPositset(const dove_eye::Positset positset);

 protected:
  void timerEvent(QTimerEvent *event) override;

 private:
  QBasicTimer timer_;

  bool has_frameset_;
  dove_eye::Frameset frameset_;

  bool has_positset_;
  dove_eye::Positset positset_;

  bool allow_drop_ = true;

  QVector<QSize> frame_sizes_;
  QVector<QSize> viewer_sizes_;

  QSize CalculateNewSize(const dove_eye::CameraIndex cam,
                         size_t frame_rows, size_t frame_cols);

  void ProcessFramesetInternal(const dove_eye::Frameset &frameset);
  void EnqueueFrameset(const dove_eye::Frameset &frameset);

  void ProcessPositsetInternal(const dove_eye::Positset positset);
  void EnqueuePositset(const dove_eye::Positset positset);
};


Q_DECLARE_METATYPE(FramesetConverter::ImageList)

#endif // FRAMESET_CONVERTER_H_
