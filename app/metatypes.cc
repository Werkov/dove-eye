#include "metatypes.h"

#include <QMetaObject>

#include "controller.h"
#include "dove_eye/calibration_data.h"
#include "dove_eye/frameset.h"
#include "dove_eye/localization.h"
#include "dove_eye/types.h"
#include "frameset_converter.h"
#include "gui/gui_mark.h"

void RegisterMetaTypes() {
  /*
   * Metatypes probably don't work with typedefs, typedefs must have explicitly
   * registered name.
   */
  qRegisterMetaType<dove_eye::CalibrationData>();

  qRegisterMetaType<dove_eye::CameraIndex>();
  qRegisterMetaType<dove_eye::CameraIndex>("dove_eye::CameraIndex");

  qRegisterMetaType<FramesetConverter::ImageList>();
  qRegisterMetaType<FramesetConverter::ImageList>("ImageList");

  qRegisterMetaType<dove_eye::Frameset>();
  qRegisterMetaType<gui::GuiMark>();
  qRegisterMetaType<gui::GuiMark>("GuiMark");

  qRegisterMetaType<dove_eye::Location>("dove_eye::Location");
  qRegisterMetaType<Controller::Mode>();
  qRegisterMetaType<Controller::UndistortMode>();
}

