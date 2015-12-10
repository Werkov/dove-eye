#include "gui/open_videos_dialog.h"

#include <QMessageBox>

#include "dove_eye/camera_video_provider.h"
#include "ui_open_videos_dialog.h"
#include "widgets/file_selector.h"

using dove_eye::CameraIndex;
using dove_eye::FileVideoProvider;

namespace gui {

OpenVideosDialog::OpenVideosDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::OpenVideosDialog) {
  connect(this, &QDialog::accepted,
          this, &OpenVideosDialog::OnAccepted);
  ui_->setupUi(this);
}

OpenVideosDialog::~OpenVideosDialog() {
}

void OpenVideosDialog::SetMaxArity(const CameraIndex max_arity) {
  // TODO same code is also in CamerasSetupDialog
  /*
   * Really? Isn't there better API to delete (direct) children of a widget?
   */
  auto children = ui_->frame->findChildren<QWidget *>();
  QList<QWidget *> to_delete;
  for (auto widget : children) {
    if (widget->parent() == ui_->frame) {
      to_delete.append(widget);
    }
  }
  for (auto widget: to_delete) {
    delete widget;
  }

  /* Now create new widgets for every provider */
  auto layout = new QVBoxLayout();

  QWidget *first_widget = nullptr;
  for (CameraIndex cam = 0; cam < max_arity; ++cam) {
    auto widget = new widgets::FileSelector(tr("Video file %0:").arg(cam));
    if (!first_widget) {
      first_widget = widget;
    }
    layout->addWidget(widget);
  }

  delete ui_->frame->layout();
  ui_->frame->setLayout(layout);
  if (first_widget) {
    first_widget->setFocus();
  }
}

void OpenVideosDialog::SetProvidersContainer(
    Application::VideoProvidersVectorOwning *ptr) {
  providers_ptr_ = ptr;
}

void OpenVideosDialog::OnAccepted() const {
  Application::VideoProvidersVector result;

  assert(providers_ptr_->size() == 0);

  bool has_error = false;
  for (auto file_selector : this->findChildren<widgets::FileSelector *>()) {
    if (file_selector->Selected()) {
      auto provider = CreateVideoProvider(file_selector->Filename());
      if (provider == nullptr) {
        has_error = true;
        QMessageBox msgBox;
        msgBox.setText(tr("Could not parse file '%0'.")
                       .arg(file_selector->Filename()));
        msgBox.exec();
      }
      /*
       * Store the pointer to the application's owning container, so that it
       * can be later properly disposed.
       * In result we just return (non owning) pointer to the recepient.
       */
      providers_ptr_->push_back(
          Application::VideoProvidersVectorOwning::value_type(provider));
      result.push_back(provider);
    }
  }

  if (!has_error && !result.empty()) {
    emit SelectedProviders(Application::kVideoFiles, result);
  }
  /*
   * If we don't emit, providers are still stored in application's owning
   * container, dispose them.
   */
  providers_ptr_->clear();
}

FileVideoProvider *OpenVideosDialog::CreateVideoProvider(const QString &filename)
    const {
  // TODO
  auto provider = new FileVideoProvider(filename.toStdString());
  if (provider->begin() == provider->end()) {
    delete provider;
    provider = nullptr;
  }

  return provider;
}

} // end namespace gui
