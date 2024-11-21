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

#include "DemView.h"

#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QApplication>
#include <QLineEdit>

#include "graphos/components/gcps/GroundControlPointsView.h"

namespace graphos
{

DemViewImp::DemViewImp(QWidget *parent)
  : DemView(parent),
    mButtonBox(new QDialogButtonBox(this))
{
    DemViewImp::initUI();
    DemViewImp::initSignalAndSlots();
}

DemViewImp::~DemViewImp()
{

}

void DemViewImp::initUI()
{
    this->setObjectName(QString("DemView"));
    this->resize(300, 200);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelCRS = new QLabel(this);
    grid_layout->addWidget(mLabelCRS, 0, 0, 1, 1);
    mLineEditCRS = new QLineEdit(this);
    mLineEditCRS->setMaximumWidth(200);
    mLineEditCRS->setDisabled(true);
    grid_layout->addWidget(mLineEditCRS, 0, 1, 1, 1);
    mQPushButtonCRS = new QPushButton(this);
    mQPushButtonCRS->setMaximumSize(QSize(31, 28));
    mQPushButtonCRS->setText("...");
    grid_layout->addWidget(mQPushButtonCRS, 0, 2, 1, 1);

    mCheckBoxDsm = new QCheckBox(this);
    grid_layout->addWidget(mCheckBoxDsm, 1, 0, 1, 2);

    mCheckBoxDtm = new QCheckBox(this);
    grid_layout->addWidget(mCheckBoxDtm, 2, 0, 1, 2);

    mLabelGSD = new QLabel(this);
    grid_layout->addWidget(mLabelGSD, 3, 0);
    mDoubleSpinBoxGSD = new QDoubleSpinBox(this);
    mDoubleSpinBoxGSD->setDecimals(3);
    mDoubleSpinBoxGSD->setSingleStep(0.1);
    grid_layout->addWidget(mDoubleSpinBoxGSD, 3, 1);

    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 4, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void DemViewImp::initSignalAndSlots()
{
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &DemViewImp::crs_change);
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &DemViewImp::update);
    connect(mQPushButtonCRS, &QAbstractButton::clicked, this, &DemViewImp::select_crs);
    connect(mCheckBoxDsm, &QCheckBox::stateChanged, this, &DemViewImp::update);
    connect(mCheckBoxDtm, &QCheckBox::stateChanged, this, &DemViewImp::update);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &DemView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void DemViewImp::clear()
{
    const QSignalBlocker signal_blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(0.1);

    const QSignalBlocker blocker_crs(mLineEditCRS);
    mLineEditCRS->clear();
}

void DemViewImp::update()
{
    mButtonBox->button(QDialogButtonBox::Apply)->setEnabled(!mLineEditCRS->text().isEmpty() && (mCheckBoxDsm->isChecked() || mCheckBoxDtm->isChecked()));
}

void DemViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("DemView", "DEM"));
    mLabelCRS->setText(QCoreApplication::translate("DemView", "Coordinate Reference System:"));
    mCheckBoxDsm->setText(QApplication::translate("DemView", "DSM"));
    mCheckBoxDtm->setText(QApplication::translate("DemView", "DTM"));
    mLabelGSD->setText(QApplication::translate("DemView", "Ground sampling distance (GSD):"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("DemView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("DemView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("DemView", "Help"));
}

auto DemViewImp::gsd() const -> double
{
    return mDoubleSpinBoxGSD->value();
}

auto DemViewImp::isDsmEnable() const -> bool
{
    return mCheckBoxDsm->isChecked();
}

auto DemViewImp::isDtmEnable() const -> bool
{
    return mCheckBoxDtm->isChecked();
}

auto DemViewImp::crs() const -> QString
{
    return mLineEditCRS->text();
}

void DemViewImp::enableDsm(bool enable)
{
    mCheckBoxDsm->setChecked(enable);
}

void DemViewImp::enableDtm(bool enable)
{
    mCheckBoxDtm->setChecked(enable);
}

void DemViewImp::setCrs(const QString &crs)
{
    mLineEditCRS->setText(crs);
}

void DemViewImp::setGsd(double gsd)
{
    const QSignalBlocker signal_blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(gsd);
}

} // namespace graphos

