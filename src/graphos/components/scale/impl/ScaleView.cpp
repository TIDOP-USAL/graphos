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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "ScaleView.h"

//#include "graphos/core/Application.h"
#include "graphos/gui/MainWindowView.h" // Esto no me gusta...

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>


namespace graphos
{

ScaleViewImp::ScaleViewImp(QWidget *parent)
  : ScaleView(parent)
{
    this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    this->initUI();
    this->initSignalAndSlots();
}

ScaleViewImp::~ScaleViewImp()
{

}

void ScaleViewImp::initUI()
{
    this->setObjectName(QString("ScaleView"));
    this->resize(380, 250);

    QGridLayout *gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mLabelDistanceReal = new QLabel(this);
    gridLayout->addWidget(mLabelDistanceReal, 0, 0, 1, 1);
    mDoubleSpinBoxDistanceReal = new QDoubleSpinBox(this);
    mDoubleSpinBoxDistanceReal->setMaximum(1000000.);
    gridLayout->addWidget(mDoubleSpinBoxDistanceReal, 0, 1, 1, 2);

    mLabelDistance = new QLabel(this);
    gridLayout->addWidget(mLabelDistance, 1, 0, 1, 1);
    mDoubleSpinBoxDistance = new QDoubleSpinBox(this);
    mDoubleSpinBoxDistance->setMaximum(1000000.);
    gridLayout->addWidget(mDoubleSpinBoxDistance, 1, 1, 1, 1);
    mPushButtonDistance = new QPushButton(this);
    mPushButtonDistance->setIcon(QIcon::fromTheme("distance"));
    mPushButtonDistance->setCheckable(true);
    gridLayout->addWidget(mPushButtonDistance, 1, 2, 1, 1);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 2, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void ScaleViewImp::initSignalAndSlots()
{
    connect(mDoubleSpinBoxDistance, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ScaleView::distanceChanged);
    connect(mPushButtonDistance, &QPushButton::toggled, this, &ScaleViewImp::enableMeasure);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &ScaleView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void ScaleViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("ScaleView", "Scale"));

    mLabelDistance->setText(QApplication::translate("DistanceView", "Length:"));
    mLabelDistanceReal->setText(QApplication::translate("DistanceView", "Length in the model:"));


    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("ScaleView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("ScaleView", "Scale"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("ScaleView", "Help"));
}

void ScaleViewImp::clear()
{

}

void ScaleViewImp::update()
{
}

double ScaleViewImp::distance() const
{
    return mDoubleSpinBoxDistance->value();
}

double ScaleViewImp::distanceReal() const
{
    return mDoubleSpinBoxDistanceReal->value();
}

void ScaleViewImp::setDistance(double distance)
{
    const QSignalBlocker blocker(mDoubleSpinBoxDistance);
    mDoubleSpinBoxDistance->setValue(distance);
}


} // namespace graphos


