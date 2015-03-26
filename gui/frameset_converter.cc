#include "frameset_converter.h"

#include <QTimerEvent>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;


namespace gui {

void FramesetConverter::SetFrameSize(const dove_eye::CameraIndex cam,
                                     const QSize size) {
  assert(cam < frame_sizes_.size());

  /*
   * Even though this method can be called from different thread,   *
   * we are omitting any locking of frame_sizes_ as unsynchronized access may
   * result in oddly resized frame only.
   */
  frame_sizes_[cam] = size;
}

void FramesetConverter::ProcessFrameset(const dove_eye::Frameset &frameset) {
  if (allow_drop_) {
    Enqueue(frameset);
  } else {
    ProcessFramesetInternal(frameset);
  }
}

void FramesetConverter::timerEvent(QTimerEvent *event) {
  if (event->timerId() != timer_.timerId()) {
    return;
  }

  ProcessFramesetInternal(frameset_);
  for (auto &frame : frameset_) {
    frame.data.release();
  }
  timer_.stop();
}

void FramesetConverter::ProcessFramesetInternal(dove_eye::Frameset frameset) {
  ImageList image_list(frameset.Size());

  for (CameraIndex cam = 0; cam < frameset.Size(); ++cam) {
    if (!frameset.IsValid(cam)) {
      continue;
    }

    /* Get own copy of frame, we'll modify it. */
    auto mat = frameset[cam].data.clone();

    /* Convert image for display. */
    if (frame_sizes_[cam].width() > 0) {
      auto &viewer_size = frame_sizes_[cam];
      double frame_ratio = static_cast<double>(mat.rows) / mat.cols;
      double viewer_ratio =
          static_cast<double>(viewer_size.height()) / viewer_size.width();

      cv::Size new_size(viewer_size.width(), viewer_size.height());

      if (viewer_ratio < frame_ratio) {
        new_size.width = new_size.height / frame_ratio;
      } else {
        new_size.height = new_size.width * frame_ratio;
      }
      cv::resize(mat, mat, new_size);
    }
    cv::cvtColor(mat, mat, CV_BGR2RGB);


    /*
     * Wrap data buffer to QImage object, together with it keep one copy of
     * cv::Mat that will ensure existence of buffer as long as QImage needs it.
     */
    image_list[cam] = QImage(mat.data, mat.cols, mat.rows, mat.step,
                             QImage::Format_RGB888, [](void *mat) {
                               delete static_cast<cv::Mat *>(mat);
                             }, new cv::Mat(mat));
    assert(image_list[cam].constBits() == mat.data);
  }

  emit ImagesetReady(image_list);
}

void FramesetConverter::Enqueue(const dove_eye::Frameset &frameset) {
  if (!frameset_[0].data.empty()) {
    DEBUG("Converter dropped a frameset");
  }

  frameset_ = frameset;

  if (!timer_.isActive()) {
    timer_.start(0, this);
  }
}


} // namespace gui

