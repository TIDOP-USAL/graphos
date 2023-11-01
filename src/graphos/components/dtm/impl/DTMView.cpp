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

void DtmViewImp::initUI()
{
    this->setObjectName(QString("DtmView"));
    this->resize(300, 200);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mLabelGSD = new QLabel(this);
    gridLayout->addWidget(mLabelGSD, 0, 0);
    mDoubleSpinBoxGSD = new QDoubleSpinBox(this);
    mDoubleSpinBoxGSD->setDecimals(3);
    mDoubleSpinBoxGSD->setSingleStep(0.1);
    gridLayout->addWidget(mDoubleSpinBoxGSD, 0, 1);

    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 1, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void DtmViewImp::initSignalAndSlots()
{
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &DtmView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void DtmViewImp::clear()
{
    const QSignalBlocker signalBlocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(0.1);
}

void DtmViewImp::update()
{
}

void DtmViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("DtmView", "DTM/DSM"));
    mLabelGSD->setText(QApplication::translate("DtmView", "Ground sampling distance (GSD):"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("DtmView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("DtmView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("DtmView", "Help"));
}

double DtmViewImp::gsd() const
{
    return mDoubleSpinBoxGSD->value();
}

void DtmViewImp::setGSD(double gsd)
{
    const QSignalBlocker signalBlocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(gsd);
}

} // namespace graphos

