#include "main_window.h"

#include "dove_eye/types.h"
#include "frameset_viewer.h"
#include "ui_main_window.h"

using dove_eye::CameraIndex;
using dove_eye::Parameters;
using gui::FramesetViewer;

namespace gui {

MainWindow::MainWindow(Application *application, QWidget *parent)
    : QMainWindow(parent),
      application_(application),
      ui_(new Ui::MainWindow),
      parameters_dialog_(new ParametersDialog(application->parameters())),
      video_providers_dialog_(new VideoProvidersDialog()) {
  ui_->setupUi(this);

  connect(application_, &Application::ChangedArity,
          this, &MainWindow::ChangeArity);

  /* Dialog connections */
  connect(video_providers_dialog_.get(), &VideoProvidersDialog::SelectedProviders,
          application_, &Application::UseVideoProviders);

  /* Menu actions */
  connect(ui_->action_modify_parameters, &QAction::triggered,
          this, &MainWindow::ModifyParameters);
  connect(ui_->action_video_providers, &QAction::triggered,
          this, &MainWindow::VideoProviders);
}

MainWindow::~MainWindow() {
}

void MainWindow::ChangeArity(const CameraIndex arity) {
  ui_->viewer->SetArity(arity);
  ui_->viewer->SetConverter(application_->converter());

  QObject::connect(application_->converter(), &FramesetConverter::ImagesetReady,
                   ui_->viewer, &FramesetViewer::SetImageset);
}

void MainWindow::ModifyParameters() {
  parameters_dialog_->LoadValues();
  parameters_dialog_->show();
}

void MainWindow::VideoProviders() {
  video_providers_dialog_->SetProviders(application_->AvailableVideoProviders());
  video_providers_dialog_->show();
}

} // end namespace gui
