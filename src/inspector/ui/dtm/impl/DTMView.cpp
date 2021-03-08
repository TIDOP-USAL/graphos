#include "DTMView.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QApplication>

namespace inspector
{
	
namespace ui
{

DtmViewImp::DtmViewImp(QWidget *parent)
  : DtmView(parent),
    mButtonBox(new QDialogButtonBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

DtmViewImp::~DtmViewImp()
{

}

void DtmViewImp::initUI()
{
  this->setObjectName(QString("DtmView"));
  //this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(380, 350);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);
  
  mLabelGSD = new QLabel(this);
  gridLayout->addWidget(mLabelGSD, 0, 0);
  mDoubleSpinBoxGSD = new QDoubleSpinBox(this); 
  mDoubleSpinBoxGSD->setDecimals(3);
  gridLayout->addWidget(mDoubleSpinBoxGSD, 0, 1);

  mLabelDtmMethod = new QLabel(this);
  gridLayout->addWidget(mLabelDtmMethod, 1, 0, 1, 1);
  mComboBoxDtmMethod = new QComboBox(this);
  gridLayout->addWidget(mComboBoxDtmMethod, 1, 1, 1, 1);

  QWidget *widgetDtm = new QWidget();
  mGridLayoutDtmMethod = new QGridLayout(widgetDtm);
  mGridLayoutDtmMethod->setContentsMargins(0, 0, 0, 0);
  gridLayout->addWidget(widgetDtm, 2, 0, 1, 2);

  //gridLayout->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding), 2, 0, 1, 2);

  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 3, 0, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void DtmViewImp::initSignalAndSlots()
{
  connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &DtmView::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &IDialogView::help);

}

void DtmViewImp::clear()
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxGSD);
  mDoubleSpinBoxGSD->setValue(0.1);
}

void DtmViewImp::update()
{
  int mdtWidgetsCount = mComboBoxDtmMethod->count();
  mLabelDtmMethod->setVisible(mdtWidgetsCount > 1);
  mComboBoxDtmMethod->setVisible(mdtWidgetsCount > 1);

}

void DtmViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("DtmView", "DTM/DSM"));
  mLabelDtmMethod->setText(QApplication::translate("DtmView", "Dtm interpolation method:"));
  mLabelGSD->setText(QApplication::translate("DtmView", "Ground sampling distance (GSD):"));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("DtmView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("DtmView", "Run"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("DtmView", "Help"));
}

double DtmViewImp::gsd() const
{
  return mDoubleSpinBoxGSD->value();
}

void DtmViewImp::addDtmMethod(QWidget *method)
{
  mComboBoxDtmMethod->addItem(method->windowTitle());
  mGridLayoutDtmMethod->addWidget(method, 0, 0, 1, 2);
  method->setVisible(false);

  this->update();
}

QString DtmViewImp::currentDtmMethod() const
{
  return mComboBoxDtmMethod->currentText();
}

void DtmViewImp::setGSD(double gsd)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxGSD);
  mDoubleSpinBoxGSD->setValue(gsd);
}

void DtmViewImp::setCurrentDtmMethod(const QString &method)
{
  const QSignalBlocker blocker(mComboBoxDtmMethod);
  mComboBoxDtmMethod->setCurrentText(method);

  for (int idx = 0; idx < mGridLayoutDtmMethod->count(); idx++){
    QLayoutItem * const item = mGridLayoutDtmMethod->itemAt(idx);
    if(dynamic_cast<QWidgetItem *>(item)){
      if (item->widget()->windowTitle().compare(method) == 0)
        item->widget()->setVisible(true);
      else
        item->widget()->setVisible(false);
    }
  }
}


} // namespace ui

} // namespace inspector
