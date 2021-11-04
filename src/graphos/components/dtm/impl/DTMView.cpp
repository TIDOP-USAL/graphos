/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "DTMView.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QRadioButton>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QApplication>

namespace graphos
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

//void DtmViewImp::onMdtSelect()
//{
//}
//
//void DtmViewImp::onMdsSelect()
//{
//}

void DtmViewImp::initUI()
{
  this->setObjectName(QString("DtmView"));
  //this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
  this->resize(380, 350);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);
  
  mRadioButtonMdt = new QRadioButton(this);
  gridLayout->addWidget(mRadioButtonMdt, 0, 0);
  mRadioButtonMds = new QRadioButton(this);
  gridLayout->addWidget(mRadioButtonMds, 0, 1);

  mLabelGSD = new QLabel(this);
  gridLayout->addWidget(mLabelGSD, 1, 0);
  mDoubleSpinBoxGSD = new QDoubleSpinBox(this); 
  mDoubleSpinBoxGSD->setDecimals(3);
  gridLayout->addWidget(mDoubleSpinBoxGSD, 1, 1);

  mLabelDtmMethod = new QLabel(this);
  gridLayout->addWidget(mLabelDtmMethod, 2, 0, 1, 1);
  mComboBoxDtmMethod = new QComboBox(this);
  gridLayout->addWidget(mComboBoxDtmMethod, 2, 1, 1, 1);

  QWidget *widgetDtm = new QWidget();
  mGridLayoutDtmMethod = new QGridLayout(widgetDtm);
  mGridLayoutDtmMethod->setContentsMargins(0, 0, 0, 0);
  gridLayout->addWidget(widgetDtm, 3, 0, 1, 2);

  //gridLayout->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding), 2, 0, 1, 2);

  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void DtmViewImp::initSignalAndSlots()
{
  //connect(mRadioButtonMdt,  &QAbstractButton::released,  this, &DtmViewImp::onMdtSelect);
  //connect(mRadioButtonMds,  &QAbstractButton::released,  this, &DtmViewImp::onMdsSelect);

  connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &DtmView::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);
}

void DtmViewImp::clear()
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxGSD);
  mDoubleSpinBoxGSD->setValue(0.1);
  mRadioButtonMdt->setChecked(true);
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
  mRadioButtonMdt->setText(QApplication::translate("DtmView", "MDT"));
  mRadioButtonMds->setText(QApplication::translate("DtmView", "MDS"));
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

bool DtmViewImp::isDSM() const
{
  return mRadioButtonMds->isChecked();
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

void DtmViewImp::setDSM(bool active)
{
  mRadioButtonMds->setChecked(active);
}

} // namespace graphos

