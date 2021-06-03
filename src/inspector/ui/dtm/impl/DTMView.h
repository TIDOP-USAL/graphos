#ifndef INSPECTOR_DTM_VIEW_H
#define INSPECTOR_DTM_VIEW_H

#include "inspector/ui/dtm/DTMView.h"

class QGridLayout;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QDoubleSpinBox;
class QRadioButton;

namespace inspector
{

namespace ui
{

class DtmViewImp
  : public DtmView
{

  Q_OBJECT

public:

  DtmViewImp(QWidget *parent = nullptr);
  ~DtmViewImp() override;

//private slots:
//
//  void onMdtSelect();
//  void onMdsSelect();

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// DtmView interface

public:

  double gsd() const override;
  void addDtmMethod(QWidget *method) override;
  QString currentDtmMethod() const override;
  bool isDSM() const override;

public slots:

  void setGSD(double gsd) override;
  void setCurrentDtmMethod(const QString &method) override;
  void setDSM(bool active) override;

private:

  QGridLayout *mGridLayoutDtmMethod;
  QLabel *mLabelGSD;
  QDoubleSpinBox *mDoubleSpinBoxGSD;
  QRadioButton *mRadioButtonMdt;
  QRadioButton *mRadioButtonMds;
  QLabel *mLabelDtmMethod;
  QComboBox *mComboBoxDtmMethod;
  QDialogButtonBox *mButtonBox;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_DTM_VIEW_H
