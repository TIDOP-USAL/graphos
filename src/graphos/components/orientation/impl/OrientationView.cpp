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

#include "OrientationView.h"

#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QApplication>

namespace graphos
{

OrientationViewImp::OrientationViewImp(QWidget *parent)
  : OrientationView(parent),
    mCheckBoxFixCalibration(new QCheckBox(this)),
    mCheckBoxFixPoses(new QCheckBox(this)),
    mCheckBoxAbsoluteOrientation(new QCheckBox(this)),
    mButtonBox(new QDialogButtonBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

void OrientationViewImp::initUI()
{
  this->setObjectName(QString("OrientationView"));
  this->resize(380, 250);

  QGridLayout *gridLayout = new QGridLayout();
  this->setLayout(gridLayout);

  gridLayout->addWidget(mCheckBoxFixCalibration, 0, 0, 1, 2);
  gridLayout->addWidget(mCheckBoxFixPoses, 1, 0, 1, 2);
  gridLayout->addWidget(mCheckBoxAbsoluteOrientation, 2, 0, 1, 2);

  mButtonBox->setOrientation(Qt::Orientation::Horizontal);
  mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  gridLayout->addWidget(mButtonBox, 3, 0, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void OrientationViewImp::initSignalAndSlots()
{
  connect(mCheckBoxFixCalibration, SIGNAL(clicked(bool)), this, SIGNAL(calibrationChange(bool)));
  connect(mCheckBoxFixPoses, SIGNAL(clicked(bool)), this, SIGNAL(posesChange(bool)));
  connect(mCheckBoxAbsoluteOrientation, SIGNAL(clicked(bool)), this, SIGNAL(absoluteOrientationChange(bool)));
  
  connect(mButtonBox,                                    &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(mButtonBox->button(QDialogButtonBox::Apply),   &QAbstractButton::clicked,   this, &OrientationView::run);
  connect(mButtonBox->button(QDialogButtonBox::Help),    &QAbstractButton::clicked,   this, &DialogView::help);
}

void OrientationViewImp::clear()
{
  mCheckBoxFixCalibration->setChecked(false);
  mCheckBoxFixPoses->setChecked(false);
  mCheckBoxAbsoluteOrientation->setChecked(false);
  mCheckBoxAbsoluteOrientation->setEnabled(false);
}

void OrientationViewImp::update()
{
}

void OrientationViewImp::retranslate()
{
  this->setWindowTitle(QApplication::translate("OrientationView", "Orientation", nullptr));
  //mCheckBoxRefinePrincipalPoint->setText(QApplication::translate("OrientationView", "Refine principal point", nullptr));
  mCheckBoxFixCalibration->setText(QApplication::translate("OrientationView", "Fix Calibration", nullptr));
  mCheckBoxFixPoses->setText(QApplication::translate("OrientationView", "Fix Poses", nullptr));
  mCheckBoxAbsoluteOrientation->setText(QApplication::translate("OrientationView", "Absolute Orientation", nullptr));
  mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("OrientationView", "Cancel"));
  mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("OrientationView", "Run"));
  mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("OrientationView", "Help"));
}

bool OrientationViewImp::fixCalibration() const
{
  return mCheckBoxFixCalibration->isChecked();
}

bool OrientationViewImp::isEnabledCalibration() const
{
  return mCheckBoxFixCalibration->isEnabled();
}

bool OrientationViewImp::absoluteOrientation() const
{
  return mCheckBoxAbsoluteOrientation->isChecked();
}

bool OrientationViewImp::isEnabledAbsoluteOrientation() const
{
  return mCheckBoxAbsoluteOrientation->isEnabled();
}

bool OrientationViewImp::fixPoses() const
{
  return mCheckBoxFixPoses->isChecked();;
}

bool OrientationViewImp::isEnabledPoses() const
{
  return mCheckBoxFixPoses->isEnabled();
}

void OrientationViewImp::setCalibration(bool active)
{
  mCheckBoxFixCalibration->setChecked(active);
}

void OrientationViewImp::enabledCalibration(bool enabled)
{
  mCheckBoxFixCalibration->setEnabled(enabled);
}

void OrientationViewImp::setAbsoluteOrientation(bool active)
{
  mCheckBoxAbsoluteOrientation->setChecked(active);
}

void OrientationViewImp::enabledAbsoluteOrientation(bool enabled)
{
  mCheckBoxAbsoluteOrientation->setEnabled(enabled);
}

void OrientationViewImp::setPoses(bool active)
{
  mCheckBoxFixPoses->setChecked(active);
}

void OrientationViewImp::enabledPoses(bool enabled)
{
  mCheckBoxFixPoses->setEnabled(enabled);
}

} // namespace graphos
