#include "main_window.h"

#include <iostream>


#include "controller.h"
#include "dove_eye/types.h"
#include "frameset_viewer.h"
#include "ui_main_window.h"


using gui::FramesetConverter;
using gui::Controller;
using gui::FramesetViewer;
using std::cerr;
using std::endl;

MainWindow::MainWindow(Controller *controller,
                       QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      converter_(new FramesetConverter(controller->Arity())),
      parameters_dialog_(new ParametersDialog()) {
  ui_->setupUi(this);
  ui_->viewer->SetArity(controller->Arity());
  ui_->viewer->SetConverter(converter_.get());

  qRegisterMetaType<FramesetConverter::ImageList>();
  qRegisterMetaType<FramesetConverter::ImageList>("ImageList");

  // TODO Should be registered somewhere else?
  //      (Must be before connections are established.)
  qRegisterMetaType<dove_eye::CameraIndex>();
  /* Metatypes probably don't work with typedefs so here's a workaround */
  qRegisterMetaType<dove_eye::CameraIndex>("dove_eye::CameraIndex");
  qRegisterMetaType<dove_eye::Frameset>();
  qRegisterMetaType<gui::GuiMark>();
  qRegisterMetaType<gui::GuiMark>("GuiMark");


  controller_thread_.start();
  converter_thread_.start();

  controller->moveToThread(&controller_thread_);
  converter_->moveToThread(&converter_thread_);

  QObject::connect(controller, &Controller::FramesetReady,
                   converter_.get(), &FramesetConverter::ProcessFrameset);
  QObject::connect(converter_.get(), &FramesetConverter::ImagesetReady,
                   ui_->viewer, &FramesetViewer::SetImageset);
  QObject::connect(converter_.get(), &FramesetConverter::MarkCreated,
                   controller, &Controller::SetMark);

  connect(ui_->action_modify_parameters, SIGNAL(triggered()), SLOT(ModifyParameters()));
  /*
   * Use invokeMethod because we are calling different thread (asynchronously)
   */
  QMetaObject::invokeMethod(controller, "Start");
}

MainWindow::~MainWindow() {
  controller_thread_.quit();
  converter_thread_.quit();
  controller_thread_.wait();
  converter_thread_.wait();
}

void MainWindow::ModifyParameters() {
  parameters_dialog_->show();
}

