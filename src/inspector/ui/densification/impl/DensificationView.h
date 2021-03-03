#ifndef INSPECTOR_DENSE_VIEW_H
#define INSPECTOR_DENSE_VIEW_H

#include "inspector/ui/densification/DensificationView.h"

class QGridLayout;
class QDialogButtonBox;
class QComboBox;
class QLabel;

namespace inspector
{

namespace ui
{


class DensificationViewImp
  : public DensificationView
{
  Q_OBJECT

public:

  explicit DensificationViewImp(QWidget *parent = nullptr);
  ~DensificationViewImp() override;

// DensificationView interface

public:

  QString currentDensificationMethod() const override;

public slots:

  void addDensification(QWidget *densification) override;
  void setCurrentDensificationMethod(const QString &densificationMethod) override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

private:

  QGridLayout *mGridLayout;
  QGridLayout *mGridLayoutDensification;
  QLabel *mLabelDensification;
  QComboBox *mComboBoxDensification;
  QDialogButtonBox *mButtonBox;

};

} // End namespace ui

} // End namespace inspector


#endif // INSPECTOR_DENSE_VIEW_H
