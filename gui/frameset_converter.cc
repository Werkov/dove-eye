#include "frameset_converter.h"

#include <QTimerEvent>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;


namespace gui {

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
    auto &mat = frameset[cam].data;

    /* Convert image for display. */
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
    DEBUG("Converter dropped a frame\n");
  }

  frameset_ = frameset;

  if (!timer_.isActive()) {
    timer_.start(0, this);
  }
}


} // namespace gui

