#ifndef GUI_PARAMETERS_DIALOG_H_
#define GUI_PARAMETERS_DIALOG_H_

#include <memory>

#include <QDialog>


namespace Ui {
class ParametersDialog;
}

class ParametersDialog : public QDialog {
  Q_OBJECT
 public:
  explicit ParametersDialog(QWidget *parent = nullptr);

  ~ParametersDialog() override;

 private:
  std::unique_ptr<Ui::ParametersDialog> ui_;

};

#endif // GUI_PARAMETERS_DIALOG_H_
