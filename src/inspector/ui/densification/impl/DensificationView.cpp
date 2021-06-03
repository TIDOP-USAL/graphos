#include "DensificationView.h"

#include <QPushButton>
#include <QApplication>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QApplication>

namespace inspector
{

namespace ui
{

DensificationViewImp::DensificationViewImp(QWidget *parent)
  : DensificationView(parent)
{
  this->initUI();
  this->initSignalAndSlots();
}

DensificationViewImp::~DensificationViewImp()
{

}

void DensificationViewImp::initUI()
{
  this->setObjectName(QString("DensificationView"));
  this->resize(443, 248);
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));

  mGridLayout = new QGridLayout(this);

  mLabelDensification = new QLabel(this);
  mGridLayout->addWidget(mLabelDensification, 0, 0, 1, 1);

  mComboBoxDensification = new QComboBox(this);
  mGridLayout->addWidget(mComboBoxDensification, 0, 1, 1, 1);

  QWidget *widgetDensification = new QWidget();
  mGridLayoutDensification = new QGridLayout(widgetDensification);
  mGridLayoutDensification->setContentsMargins(0, 0, 0, 0);
  mGridLayout->addWidget(widgetDensification, 1, 0, 1, 2);

  mButtonBox = new QDialogButtonBox(this);
  mButtonBox->setOrientation(Qt::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  mGridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void DensificationViewImp::initSignalAndSlots()
{
  connect(mComboBoxDensification,  &QComboBox::currentTextChanged, this, &DensificationView::densificationChanged);

  connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &DensificationView::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);
}

void DensificationViewImp::clear()
{

}

void DensificationViewImp::update()
{

}

void DensificationViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("DensificationView", "Densification", nullptr));
  mLabelDensification->setText(QApplication::translate("DensificationView", "Preprocess:", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("FeatureExtractorView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("FeatureExtractorView", "Run"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("FeatureExtractorView", "Help"));
}

QString DensificationViewImp::currentDensificationMethod() const
{
  return mComboBoxDensification->currentText();
}

void DensificationViewImp::addDensification(QWidget *densification)
{
  mComboBoxDensification->addItem(densification->windowTitle());
  mGridLayoutDensification->addWidget(densification, 1, 0, 1, 2);
  densification->setVisible(false);
}

void DensificationViewImp::setCurrentDensificationMethod(const QString &densificationMethod)
{
  const QSignalBlocker blocker(mComboBoxDensification);
  mComboBoxDensification->setCurrentText(densificationMethod);

  for (int idx = 0; idx < mGridLayoutDensification->count(); idx++){
    QLayoutItem *item = mGridLayoutDensification->itemAt(idx);
    if(dynamic_cast<QWidgetItem *>(item)){
      if (item->widget()->windowTitle().compare(densificationMethod) == 0)
        item->widget()->setVisible(true);
      else
        item->widget()->setVisible(false);
    }
  }
}

} // End namespace ui

} // End namespace inspector
