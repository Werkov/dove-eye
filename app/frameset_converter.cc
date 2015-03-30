#include "frameset_converter.h"

#include <QTimerEvent>

#include "dove_eye/logging.h"

using dove_eye::CameraIndex;
using gui::GuiMark;


void FramesetConverter::SetFrameSize(const dove_eye::CameraIndex cam,
                                     const QSize size) {
  assert(cam < viewer_sizes_.size());

  /*
   * Even though this method can be called from different thread,
   * we are omitting any locking of viewer_sizes_ as unsynchronized access may
   * result in oddly resized frame only.
   */
  viewer_sizes_[cam] = size;
}

void FramesetConverter::PropagateMark(const dove_eye::CameraIndex cam,
                                      const GuiMark mark) {

  /*
   * Access to frame/viewer sizes is unsynchronized.
   * It may result only incorrectly calculated mark coordinates
   * (which is not fatal).
   */
  auto frame_size = frame_sizes_[cam];
  auto viewer_size = viewer_sizes_[cam];

  double frame_ratio =
          static_cast<double>(frame_size.height()) / frame_size.width();
  double viewer_ratio =
          static_cast<double>(viewer_size.height()) / viewer_size.width();

  double scale = 1;
  if (viewer_ratio > frame_ratio) {
    scale = static_cast<double>(frame_size.width()) / viewer_size.width();
  } else {
    scale = static_cast<double>(frame_size.height()) / viewer_size.height();
  }

  
  GuiMark new_mark;
  new_mark.pos = mark.pos * scale;

  /* Check boundaries, assume zero-based indexing */
  if (new_mark.pos.x() < frame_size.width() &&
      new_mark.pos.y() < frame_size.height()) {
    emit MarkCreated(cam, new_mark);
  }
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
  ImageList image_list(frameset.Arity());

  for (CameraIndex cam = 0; cam < frameset.Arity(); ++cam) {
    if (!frameset.IsValid(cam)) {
      continue;
    }

    /* Get own copy of frame, we'll modify it. */
    auto mat = frameset[cam].data.clone();
    if (!mat.data) {
      DEBUG("Empty data from cam %i", cam);
      continue;
    }

    /*
     * Update frame size, we do it every frame, however, it's not actually
     * assumed that frame size would change between frames.
     */
    frame_sizes_[cam].setWidth(mat.cols);
    frame_sizes_[cam].setHeight(mat.rows);

    /* Convert image for display. */
    if (viewer_sizes_[cam].width() > 0) {
      auto &viewer_size = viewer_sizes_[cam];
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

    if (mat.channels() == 1) {
      cv::cvtColor(mat, mat, CV_GRAY2BGR);
    } else if (mat.channels() == 3) {
      cv::cvtColor(mat, mat, CV_BGR2RGB);
    } else {
      ERROR("Unexpected no. of channels (%i) in cam %i frame.",
            mat.channels(), cam);
    }


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



