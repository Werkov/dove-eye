#include "parameters_dialog.h"

#include <QAbstractButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include "ui_parameters_dialog.h"

using dove_eye::Parameters;

ParametersDialog::ParametersDialog(Parameters &parameters, QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::ParametersDialog),
      parameters_(parameters) {
  param_widgets_.fill(nullptr);

  ui_->setupUi(this);
  ui_->button_box->button(QDialogButtonBox::Apply)->setDefault(true);
  CreateControls();

  connect(ui_->button_box, &QDialogButtonBox::clicked,
          [=](QAbstractButton *button) {
            switch (ui_->button_box->buttonRole(button)) {
              case QDialogButtonBox::ResetRole:
                this->LoadValues();
                break;
              case QDialogButtonBox::ApplyRole:
                this->StoreValues();
                break;
              default:
                /* Ignore other buttons */
                return;
            }
          });
}

ParametersDialog::~ParametersDialog() {
}

void ParametersDialog::LoadValues() {
  for (auto param : parameters_) {
    auto spinBox = dynamic_cast<QDoubleSpinBox *>(param_widgets_[param.key]);
    spinBox->setValue(parameters_.Get(param.key));
  }
}

void ParametersDialog::StoreValues() {
  for (auto param : parameters_) {
    auto spinBox = dynamic_cast<QDoubleSpinBox *>(param_widgets_[param.key]);
    parameters_.Set(param.key, spinBox->value());
  }
}

void ParametersDialog::showEvent(QShowEvent *e) {
}

void ParametersDialog::CreateControls() {
  const auto kSteps = 20;

  auto layout = new QVBoxLayout();

  for (auto param : parameters_) {
    auto labelBox = new QLabel(tr("%0:")
                               .arg(QString(param.name.c_str())));
    layout->addWidget(labelBox);
    
    auto spinBox = new QDoubleSpinBox();
    layout->addWidget(spinBox);
    spinBox->setMinimum(param.min_value);
    spinBox->setMaximum(param.max_value);
    spinBox->setSingleStep((param.max_value - param.min_value) / kSteps);

    if (param.unit != "") {
      spinBox->setSuffix((" " + param.unit).c_str());
    }

    param_widgets_[param.key] = spinBox;
  }

  ui_->container->setLayout(layout);
}
