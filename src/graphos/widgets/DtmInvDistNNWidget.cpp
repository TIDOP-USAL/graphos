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

#include "DtmInvDistNNWidget.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>

namespace graphos
{


DtmInvDistNNWidget::DtmInvDistNNWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
}




DtmInvDistNNWidgetImp::DtmInvDistNNWidgetImp(QWidget *parent)
  : DtmInvDistNNWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelPower(new QLabel(this)),
    mDoubleSpinBoxPower(new QDoubleSpinBox(this)),
    mLabelSmoothing(new QLabel(this)),
    mDoubleSpinBoxSmoothing(new QDoubleSpinBox(this)),
    mLabelRadius(new QLabel(this)),
    mDoubleSpinBoxRadius(new QDoubleSpinBox(this)),
    mLabelMaxPoints(new QLabel(this)),
    mSpinBoxMaxPoints(new QSpinBox(this)),
    mLabelMinPoints(new QLabel(this)),
    mSpinBoxMinPoints(new QSpinBox(this))
{
  DtmInvDistNNWidgetImp::initUI();
  DtmInvDistNNWidgetImp::initSignalAndSlots();
}

DtmInvDistNNWidgetImp::~DtmInvDistNNWidgetImp()
{

}

void DtmInvDistNNWidgetImp::initUI()
{
  this->setObjectName(QStringLiteral("DtmInvDistNN"));
  this->setWindowTitle("Inverse distance to a power with nearest neighbor searching");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelPower, 0, 0);
  mDoubleSpinBoxPower->setSingleStep(1.);
  propertiesLayout->addWidget(mDoubleSpinBoxPower, 0, 1);

  propertiesLayout->addWidget(mLabelSmoothing, 1, 0);
  mDoubleSpinBoxSmoothing->setDecimals(3);
  mDoubleSpinBoxSmoothing->setSingleStep(0.01);
  propertiesLayout->addWidget(mDoubleSpinBoxSmoothing, 1, 1);
  
  propertiesLayout->addWidget(mLabelRadius, 2, 0);
  mDoubleSpinBoxRadius->setDecimals(3);
  mDoubleSpinBoxRadius->setSingleStep(0.01);
  propertiesLayout->addWidget(mDoubleSpinBoxRadius, 2, 1);

  propertiesLayout->addWidget(mLabelMaxPoints, 3, 0);
  propertiesLayout->addWidget(mSpinBoxMaxPoints, 3, 1);

  propertiesLayout->addWidget(mLabelMinPoints, 4, 0);
  propertiesLayout->addWidget(mSpinBoxMinPoints, 4, 1);

  DtmInvDistNNWidgetImp::retranslate();
  DtmInvDistNNWidgetImp::clear(); /// set default values
  DtmInvDistNNWidgetImp::update();
}

void DtmInvDistNNWidgetImp::initSignalAndSlots()
{
  connect(mDoubleSpinBoxPower,      SIGNAL(valueChanged(double)), this, SIGNAL(powerChange(double)));
  connect(mDoubleSpinBoxSmoothing,  SIGNAL(valueChanged(double)), this, SIGNAL(smoothingChange(double)));
  connect(mDoubleSpinBoxRadius,     SIGNAL(valueChanged(double)), this, SIGNAL(radiusChange(double)));
  connect(mSpinBoxMaxPoints,        SIGNAL(valueChanged(int)),    this, SIGNAL(maxPointsChange(int)));
  connect(mSpinBoxMinPoints,        SIGNAL(valueChanged(int)),    this, SIGNAL(minPointsChange(int)));
}

void DtmInvDistNNWidgetImp::clear()
{
  const QSignalBlocker signalBlockerPower(mDoubleSpinBoxPower);
  const QSignalBlocker signalBlockerSmoothing(mDoubleSpinBoxSmoothing);
  const QSignalBlocker signalBlockerRadius(mDoubleSpinBoxRadius);
  const QSignalBlocker signalBlockerMaxPoints(mSpinBoxMaxPoints);
  const QSignalBlocker signalBlockerMinPoints(mSpinBoxMinPoints);

  mDoubleSpinBoxPower->setValue(2.0);
  mDoubleSpinBoxSmoothing->setValue(0.0);
  mDoubleSpinBoxRadius->setValue(1.0);
  mSpinBoxMaxPoints->setValue(0);
  mSpinBoxMinPoints->setValue(0);

}

void DtmInvDistNNWidgetImp::update()
{

}

void DtmInvDistNNWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("DtmInvDistNNWidget", "Inverse distance to a power with nearest neighbor searching", nullptr));
  mLabelPower->setText(QApplication::translate("DtmInvDistNNWidget", "Power:"));
  mLabelSmoothing->setText(QApplication::translate("DtmInvDistNNWidget", "Smoothing:"));
  mLabelRadius->setText(QApplication::translate("DtmInvDistNNWidget", "Radius:"));
  mLabelMaxPoints->setText(QApplication::translate("DtmInvDistNNWidget", "Maximum Points:"));
  mLabelMinPoints->setText(QApplication::translate("DtmInvDistNNWidget", "Minimun Points:"));
}

double DtmInvDistNNWidgetImp::power() const
{
  return mDoubleSpinBoxPower->value();
}

double DtmInvDistNNWidgetImp::smoothing() const
{
  return mDoubleSpinBoxSmoothing->value();
}

double DtmInvDistNNWidgetImp::radius() const
{
  return mDoubleSpinBoxRadius->value();
}

int DtmInvDistNNWidgetImp::maxPoints() const
{
  return mSpinBoxMaxPoints->value();
}

int DtmInvDistNNWidgetImp::minPoints() const
{
  return mSpinBoxMinPoints->value();
}

void DtmInvDistNNWidgetImp::setPower(double power)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxPower);
  mDoubleSpinBoxPower->setValue(power);
}

void DtmInvDistNNWidgetImp::setSmoothing(double smoothing)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxSmoothing);
  mDoubleSpinBoxSmoothing->setValue(smoothing);
}

void DtmInvDistNNWidgetImp::setRadius(double radius)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxRadius);
  mDoubleSpinBoxRadius->setValue(radius);
}

void DtmInvDistNNWidgetImp::setMaxPoints(int maxPoints)
{
  const QSignalBlocker signalBlocker(mSpinBoxMaxPoints);
  mSpinBoxMaxPoints->setValue(maxPoints);
}

void DtmInvDistNNWidgetImp::setMinPoints(int minPoints)
{
  const QSignalBlocker signalBlocker(mSpinBoxMinPoints);
  mSpinBoxMinPoints->setValue(minPoints);
}


} // namespace graphos


