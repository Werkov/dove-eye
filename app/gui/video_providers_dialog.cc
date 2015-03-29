#include "gui/video_providers_dialog.h"

#include "ui_video_providers_dialog.h"

namespace gui {

VideoProvidersDialog::VideoProvidersDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::VideoProvidersDialog) {
}

VideoProvidersDialog::~VideoProvidersDialog() {
}

} // end namespace gui
