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

namespace graphos
{

DtmViewImp::DtmViewImp(QWidget *parent)
  : DtmView(parent),
    mButtonBox(new QDialogButtonBox(this))
{
    DtmViewImp::initUI();
    DtmViewImp::initSignalAndSlots();
}

DtmViewImp::~DtmViewImp()
{

}

void DtmViewImp::initUI()
{
    this->setObjectName(QString("DtmView"));
    this->resize(300, 200);

    QGridLayout *grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mCheckBoxMDS = new QCheckBox(this);
    grid_layout->addWidget(mCheckBoxMDS, 0, 0, 1, 2);

    mCheckBoxMDT = new QCheckBox(this);
    grid_layout->addWidget(mCheckBoxMDT, 1, 0, 1, 2);

    mLabelGSD = new QLabel(this);
    grid_layout->addWidget(mLabelGSD, 2, 0);
    mDoubleSpinBoxGSD = new QDoubleSpinBox(this);
    mDoubleSpinBoxGSD->setDecimals(3);
    mDoubleSpinBoxGSD->setSingleStep(0.1);
    grid_layout->addWidget(mDoubleSpinBoxGSD, 2, 1);

    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 3, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void DtmViewImp::initSignalAndSlots()
{
    connect(mCheckBoxMDS, &QCheckBox::stateChanged, this, &DtmViewImp::update);
    connect(mCheckBoxMDT, &QCheckBox::stateChanged, this, &DtmViewImp::update);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &DtmView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void DtmViewImp::clear()
{
    const QSignalBlocker signal_blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(0.1);
}

void DtmViewImp::update()
{
    mButtonBox->button(QDialogButtonBox::Apply)->setEnabled(mCheckBoxMDS->isChecked() || mCheckBoxMDT->isChecked());
}

void DtmViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("DtmView", "DEM"));
    mCheckBoxMDS->setText(QApplication::translate("DtmView", "DSM"));
    mCheckBoxMDT->setText(QApplication::translate("DtmView", "DTM"));
    mLabelGSD->setText(QApplication::translate("DtmView", "Ground sampling distance (GSD):"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("DtmView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("DtmView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("DtmView", "Help"));
}

auto DtmViewImp::gsd() const -> double
{
    return mDoubleSpinBoxGSD->value();
}

auto DtmViewImp::isMdsEnable() const -> bool
{
    return mCheckBoxMDS->isChecked();
}

auto DtmViewImp::isMdtEnable() const -> bool
{
    return mCheckBoxMDT->isChecked();
}

void DtmViewImp::enableMds(bool enable)
{
    mCheckBoxMDS->setChecked(enable);
}

void DtmViewImp::enableMdt(bool enable)
{
    mCheckBoxMDT->setChecked(enable);
}

void DtmViewImp::setGsd(double gsd)
{
    const QSignalBlocker signal_blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(gsd);
}

} // namespace graphos

