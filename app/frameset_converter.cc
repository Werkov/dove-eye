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

  
  GuiMark new_mark(mark);
  new_mark.press_pos *= scale;
  new_mark.release_pos *= scale;

  /* Check boundaries, assume zero-based indexing */
  QRect frame_rect(0, 0, frame_size.width(), frame_size.height());
  if (frame_rect.contains(new_mark.press_pos, true) &&
      frame_rect.contains(new_mark.release_pos, true)) {
    emit MarkCreated(cam, new_mark);
  }
}

void FramesetConverter::ProcessFrameset(const dove_eye::Frameset &frameset) {
  if (allow_drop_) {
    EnqueueFrameset(frameset);
  } else {
    ProcessFramesetInternal(frameset);
  }
}

void FramesetConverter::ProcessPositset(const dove_eye::Positset positset) {
  if (allow_drop_) {
    EnqueuePositset(positset);
  } else {
    ProcessPositsetInternal(positset);
  }
}

void FramesetConverter::timerEvent(QTimerEvent *event) {
  if (event->timerId() != timer_.timerId()) {
    return;
  }

  if (has_frameset_) {
    ProcessFramesetInternal(frameset_);
    for (auto &frame : frameset_) {
      frame.data.release();
    }
    has_frameset_ = false;
  }

  if (has_positset_) {
    ProcessPositsetInternal(positset_);
    has_positset_ = false;
  }

  timer_.stop();
}

void FramesetConverter::ProcessFramesetInternal(
    const dove_eye::Frameset &frameset) {
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
    if (viewer_sizes_[cam].width() == 0) {
      continue;
    }

    auto new_size = CalculateNewSize(cam, mat.rows, mat.cols);
    cv::Size cv_new_size(new_size.width(), new_size.height());

    cv::resize(mat, mat, cv_new_size);

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

void FramesetConverter::EnqueueFrameset(const dove_eye::Frameset &frameset) {
  frameset_ = frameset;
  has_frameset_ = true;

  if (!timer_.isActive()) {
    timer_.start(0, this);
  }
}

void FramesetConverter::ProcessPositsetInternal(const dove_eye::Positset positset) {
  dove_eye::Positset result(positset.Arity());
  ImageList image_list(positset.Arity());

  for (CameraIndex cam = 0; cam < positset.Arity(); ++cam) {
    if (!positset.IsValid(cam) || !frameset_.IsValid(cam)) {
      continue;
    }

    if (viewer_sizes_[cam].width() == 0) {
      continue;
    }

    /*
     * Use image dimensions from frameset processing.
     */
    auto &size = frame_sizes_[cam];
    auto new_size = CalculateNewSize(cam, size.height(), size.width());
    double scale = static_cast<double>(new_size.height()) / size.height();
    

    result[cam] = scale * positset[cam];
    result.SetValid(cam, true);
  }

  emit PositsetReady(result);
}

void FramesetConverter::EnqueuePositset(const dove_eye::Positset positset) {
  positset_ = positset;
  has_positset_ = true;

  if (!timer_.isActive()) {
    timer_.start(0, this);
  }
}

QSize FramesetConverter::CalculateNewSize(const dove_eye::CameraIndex cam,
                                          size_t frame_rows,
                                          size_t frame_cols) {
  double frame_ratio = static_cast<double>(frame_rows) / frame_cols;
  auto &viewer_size = viewer_sizes_[cam];
  double viewer_ratio =
      static_cast<double>(viewer_size.height()) / viewer_size.width();

  QSize new_size(viewer_size.width(), viewer_size.height());

  if (viewer_ratio < frame_ratio) {
    new_size.setWidth(new_size.height() / frame_ratio);
  } else {
    new_size.setHeight(new_size.width() * frame_ratio);
  }

  return new_size;
}

