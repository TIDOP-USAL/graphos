/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
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
    mCheckBoxAbsoluteOrientation(new QCheckBox(this)),
    mCheckBoxRtkPositioningAccuracy(new QCheckBox(this)),
    mButtonBox(new QDialogButtonBox(this))
{
    OrientationViewImp::initUI();
    OrientationViewImp::initSignalAndSlots();
}

void OrientationViewImp::initUI()
{
    this->setObjectName(QString("OrientationView"));
    this->resize(380, 250);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    grid_layout->addWidget(mCheckBoxFixCalibration, 0, 0, 1, 2);
    grid_layout->addWidget(mCheckBoxAbsoluteOrientation, 1, 0, 1, 2);
    grid_layout->addWidget(mCheckBoxRtkPositioningAccuracy, 2, 0, 1, 2);

    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 3, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void OrientationViewImp::initSignalAndSlots()
{
    connect(mCheckBoxFixCalibration, SIGNAL(clicked(bool)), this, SIGNAL(calibrationChange(bool)));
    connect(mCheckBoxAbsoluteOrientation, SIGNAL(clicked(bool)), this, SIGNAL(absoluteOrientationChange(bool)));

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &OrientationView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void OrientationViewImp::clear()
{
    mCheckBoxFixCalibration->setChecked(false);
    mCheckBoxAbsoluteOrientation->setChecked(false);
    mCheckBoxAbsoluteOrientation->setEnabled(false);
    mCheckBoxRtkPositioningAccuracy->setChecked(false);
    mCheckBoxRtkPositioningAccuracy->setEnabled(false);
}

void OrientationViewImp::update()
{
}

void OrientationViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("OrientationComponent", "Orientation", nullptr));
    mCheckBoxFixCalibration->setText(QApplication::translate("OrientationComponent", "Fix Calibration", nullptr));
    mCheckBoxRtkPositioningAccuracy->setText(QApplication::translate("OrientationComponent", "RTK positioning accurary from EXIF", nullptr));
    mCheckBoxAbsoluteOrientation->setText(QApplication::translate("OrientationComponent", "Absolute Orientation", nullptr));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("OrientationComponent", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("OrientationComponent", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("OrientationComponent", "Help"));
}

auto OrientationViewImp::fixCalibration() const -> bool
{
    return mCheckBoxFixCalibration->isChecked();
}

auto OrientationViewImp::absoluteOrientation() const -> bool
{
    return mCheckBoxAbsoluteOrientation->isChecked();
}

auto OrientationViewImp::rtkPositioningAccuracy() const -> bool
{
    return mCheckBoxRtkPositioningAccuracy->isChecked();
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

void OrientationViewImp::setRtkPositioningAccuracy(bool active)
{
    mCheckBoxRtkPositioningAccuracy->setChecked(active);
}

void OrientationViewImp::enabledRtkPositioningAccuracy(bool enabled)
{
    mCheckBoxRtkPositioningAccuracy->setEnabled(enabled);
}


} // namespace graphos
