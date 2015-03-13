#include "main_window.h"

#include <iostream>


#include "dove_eye/frameset.h"
#include "frameset_provider.h"
#include "frameset_viewer.h"
#include "ui_main_window.h"


using gui::FramesetConverter;
using gui::FramesetProvider;
using gui::FramesetViewer;
using std::cerr;
using std::endl;

typedef FramesetProvider::InnerFrameProvider InnerFrameProvider;
Q_DECLARE_METATYPE(InnerFrameProvider *);

MainWindow::MainWindow(FramesetProvider::InnerFrameProvider *aggregator,
                       QWidget *parent)
    : QMainWindow(parent),
      ui_(new Ui::MainWindow),
      provider_(new FramesetProvider(aggregator->width())),
      converter_(new FramesetConverter(aggregator->width())) {
  ui_->setupUi(this);
  ui_->viewer->SetWidth(aggregator->width());
  ui_->viewer->SetConverter(converter_.get());

  qRegisterMetaType<InnerFrameProvider *>();
  qRegisterMetaType<FramesetConverter::ImageList>();
  qRegisterMetaType<FramesetConverter::ImageList>("ImageList");
  qRegisterMetaType<dove_eye::Frameset>();


  provider_thread_.start();
  converter_thread_.start();

  provider_->moveToThread(&provider_thread_);
  converter_->moveToThread(&converter_thread_);

  QObject::connect(provider_.get(), &FramesetProvider::FramesetReady,
                   converter_.get(), &FramesetConverter::ProcessFrameset);
  QObject::connect(converter_.get(), &FramesetConverter::ImagesetReady,
                   ui_->viewer, &FramesetViewer::SetImageset);



  QMetaObject::invokeMethod(provider_.get(), "Start",
                            Q_ARG(InnerFrameProvider *, aggregator));
}

MainWindow::~MainWindow() {
  provider_thread_.quit();
  converter_thread_.quit();
  provider_thread_.wait();
  converter_thread_.wait();
}


