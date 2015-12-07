#ifndef WIDGETS_FILE_SELECTOR_H_
#define WIDGETS_FILE_SELECTOR_H_

#include <memory>

#include <QString>
#include <QWidget>


namespace Ui {
class FileSelector;
}

namespace widgets {

class FileSelector : public QWidget {
  Q_OBJECT
 public:
  explicit FileSelector(const QString &label, QWidget *parent = nullptr);

  ~FileSelector() override;

  bool Selected() const;

  QString Filename() const;

  const QString Label() const;

  void SetLabel(const QString &label);

 private slots:
  void ClearClicked();
  void SelectClicked();

 private:
  std::unique_ptr<Ui::FileSelector> ui_;

};

} // end namespace widgets

#endif // WIDGETS_FILE_SELECTOR_H_
