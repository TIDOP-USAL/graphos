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

#include "OrthophotoView.h"

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>


namespace graphos
{

OrthophotoViewImp::OrthophotoViewImp(QWidget *parent)
  : OrthophotoView(parent)
{
    OrthophotoViewImp::initUI();
    OrthophotoViewImp::initSignalAndSlots();
}

OrthophotoViewImp::~OrthophotoViewImp()
{
}

void OrthophotoViewImp::initUI()
{
    this->setObjectName(QString("OrthophotoView"));
    this->resize(380, 250);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelResolution = new QLabel(this);
    grid_layout->addWidget(mLabelResolution, 0, 0, 1, 1);
    mDoubleSpinBoxResolution = new QDoubleSpinBox(this);
    mDoubleSpinBoxResolution->setSingleStep(0.1);
    grid_layout->addWidget(mDoubleSpinBoxResolution, 0, 1, 1, 1);


    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 7, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void OrthophotoViewImp::initSignalAndSlots()
{
    connect(mDoubleSpinBoxResolution, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &OrthophotoView::resolutionChanged);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &OrthophotoView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void OrthophotoViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("OrthophotoView", "Orthophoto"));

    mLabelResolution->setText(QApplication::translate("ResolutionView", "Resolution (m):"));


    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("BilateralFilterView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("BilateralFilterView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("BilateralFilterView", "Help"));
}

void OrthophotoViewImp::clear()
{
    mDoubleSpinBoxResolution->setValue(0.05);
}

void OrthophotoViewImp::update()
{}

auto OrthophotoViewImp::gsd() const -> double
{
    return mDoubleSpinBoxResolution->value();
}


void OrthophotoViewImp::setGSD(double gsd)
{
    const QSignalBlocker blocker(mDoubleSpinBoxResolution);
    mDoubleSpinBoxResolution->setValue(gsd);
}



} // namespace graphos


