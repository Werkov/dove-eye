#include "parameters_dialog.h"

#include "ui_parameters_dialog.h"

ParametersDialog::ParametersDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::ParametersDialog) {
  ui_->setupUi(this);

}

ParametersDialog::~ParametersDialog() {
}
