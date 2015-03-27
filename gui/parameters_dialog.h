#ifndef GUI_PARAMETERS_DIALOG_H_
#define GUI_PARAMETERS_DIALOG_H_

#include <array>
#include <memory>

#include <QDialog>
#include <QShowEvent>

#include "dove_eye/parameters.h"

namespace Ui {
class ParametersDialog;
}

class ParametersDialog : public QDialog {
  Q_OBJECT
 public:
  explicit ParametersDialog(dove_eye::Parameters &parameters,
                            QWidget *parent = nullptr);

  ~ParametersDialog() override;

 public slots:
  void LoadValues();

  void StoreValues();
 
 protected:
  void showEvent(QShowEvent *e) override;

 private:
  std::unique_ptr<Ui::ParametersDialog> ui_;

  dove_eye::Parameters &parameters_;

  std::array<QWidget *, dove_eye::Parameters::_MAX_KEY> param_widgets_;

  void CreateControls();
};

#endif // GUI_PARAMETERS_DIALOG_H_
