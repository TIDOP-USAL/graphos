#ifndef GRAPHOS_DENSE_VIEW_H
#define GRAPHOS_DENSE_VIEW_H

#include "graphos/ui/densification/DensificationView.h"

class QGridLayout;
class QDialogButtonBox;
class QComboBox;
class QLabel;

namespace graphos
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

// DialogView interface

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

} // End namespace graphos


#endif // GRAPHOS_DENSE_VIEW_H
