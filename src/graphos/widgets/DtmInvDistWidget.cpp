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

#include "DtmInvDistWidget.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>

namespace graphos
{

DtmInvDistWidgetImp::DtmInvDistWidgetImp(QWidget *parent)
  : DtmInvDistWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelPower(new QLabel(this)),
    mDoubleSpinBoxPower(new QDoubleSpinBox(this)),
    mLabelSmoothing(new QLabel(this)),
    mDoubleSpinBoxSmoothing(new QDoubleSpinBox(this)),
    mLabelRadius1(new QLabel(this)),
    mDoubleSpinBoxRadius1(new QDoubleSpinBox(this)),
    mLabelRadius2(new QLabel(this)),
    mDoubleSpinBoxRadius2(new QDoubleSpinBox(this)),
    mLabelAngle(new QLabel(this)),
    mDoubleSpinBoxAngle(new QDoubleSpinBox(this)),
    mLabelMaxPoints(new QLabel(this)),
    mSpinBoxMaxPoints(new QSpinBox(this)),
    mLabelMinPoints(new QLabel(this)),
    mSpinBoxMinPoints(new QSpinBox(this))
{
  DtmInvDistWidgetImp::initUI();
  DtmInvDistWidgetImp::initSignalAndSlots();
}

DtmInvDistWidgetImp::~DtmInvDistWidgetImp()
{

}

void DtmInvDistWidgetImp::initUI()
{
  this->setObjectName(QStringLiteral("DtmInvDist"));
  this->setWindowTitle("Inverse distance to a power");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelPower, 0, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxPower, 0, 1);

  propertiesLayout->addWidget(mLabelSmoothing, 1, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxSmoothing, 1, 1);

  propertiesLayout->addWidget(mLabelRadius1, 2, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxRadius1, 2, 1);

  propertiesLayout->addWidget(mLabelRadius2, 3, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxRadius2, 3, 1);

  propertiesLayout->addWidget(mLabelAngle, 4, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxAngle, 4, 1);

  propertiesLayout->addWidget(mLabelMaxPoints, 5, 0);
  propertiesLayout->addWidget(mSpinBoxMaxPoints, 5, 1);

  propertiesLayout->addWidget(mLabelMinPoints, 6, 0);
  propertiesLayout->addWidget(mSpinBoxMinPoints, 6, 1);

  DtmInvDistWidgetImp::retranslate();
  DtmInvDistWidgetImp::clear(); /// set default values
  DtmInvDistWidgetImp::update();
}

void DtmInvDistWidgetImp::initSignalAndSlots()
{
  connect(mDoubleSpinBoxPower,      SIGNAL(valueChanged(double)), this, SIGNAL(powerChange(double)));
  connect(mDoubleSpinBoxSmoothing,  SIGNAL(valueChanged(double)), this, SIGNAL(smoothingChange(double)));
  connect(mDoubleSpinBoxRadius1,    SIGNAL(valueChanged(double)), this, SIGNAL(radius1Change(double)));
  connect(mDoubleSpinBoxRadius2,    SIGNAL(valueChanged(double)), this, SIGNAL(radius2Change(double)));
  connect(mDoubleSpinBoxAngle,      SIGNAL(valueChanged(double)), this, SIGNAL(angleChange(double)));
  connect(mSpinBoxMaxPoints,        SIGNAL(valueChanged(int)),    this, SIGNAL(maxPointsChange(int)));
  connect(mSpinBoxMinPoints,        SIGNAL(valueChanged(int)),    this, SIGNAL(minPointsChange(int)));
}

void DtmInvDistWidgetImp::clear()
{
  const QSignalBlocker signalBlockerPower(mDoubleSpinBoxPower);
  const QSignalBlocker signalBlockerSmoothing(mDoubleSpinBoxSmoothing);
  const QSignalBlocker signalBlockerRadius1(mDoubleSpinBoxRadius1);
  const QSignalBlocker signalBlockerRadius2(mDoubleSpinBoxRadius2);
  const QSignalBlocker signalBlockerAngle(mDoubleSpinBoxAngle);
  const QSignalBlocker signalBlockerMaxPoints(mSpinBoxMaxPoints);
  const QSignalBlocker signalBlockerMinPoints(mSpinBoxMinPoints);

  mDoubleSpinBoxPower->setValue(2.0);
  mDoubleSpinBoxSmoothing->setValue(0.0);
  mDoubleSpinBoxRadius1->setValue(0.0);
  mDoubleSpinBoxRadius2->setValue(0.0);
  mDoubleSpinBoxAngle->setValue(0.0);
  mSpinBoxMaxPoints->setValue(0);
  mSpinBoxMinPoints->setValue(0);

}

void DtmInvDistWidgetImp::update()
{

}

void DtmInvDistWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("DtmInvDistWidget", "Inverse distance to a power", nullptr));
  mLabelPower->setText(QApplication::translate("DtmInvDistWidget", "Power:"));
  mLabelSmoothing->setText(QApplication::translate("DtmInvDistWidget", "Smoothing:"));
  mLabelRadius1->setText(QApplication::translate("DtmInvDistWidget", "Radius 1:"));
  mLabelRadius2->setText(QApplication::translate("DtmInvDistWidget", "Radius 2:"));
  mLabelAngle->setText(QApplication::translate("DtmInvDistWidget", "Angle:"));
  mLabelMaxPoints->setText(QApplication::translate("DtmInvDistWidget", "Maximum Points:"));
  mLabelMinPoints->setText(QApplication::translate("DtmInvDistWidget", "Minimun Points:"));
}

double DtmInvDistWidgetImp::power() const
{
  return mDoubleSpinBoxPower->value();
}

double DtmInvDistWidgetImp::smoothing() const
{
  return mDoubleSpinBoxSmoothing->value();
}

double DtmInvDistWidgetImp::radius1() const
{
  return mDoubleSpinBoxRadius1->value();
}

double DtmInvDistWidgetImp::radius2() const
{
  return mDoubleSpinBoxRadius2->value();
}

double DtmInvDistWidgetImp::angle() const
{
  return mDoubleSpinBoxAngle->value();
}

int DtmInvDistWidgetImp::maxPoints() const
{
  return mSpinBoxMaxPoints->value();
}

int DtmInvDistWidgetImp::minPoints() const
{
  return mSpinBoxMinPoints->value();
}

void DtmInvDistWidgetImp::setPower(double power)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxPower);
  mDoubleSpinBoxPower->setValue(power);
}

void DtmInvDistWidgetImp::setSmoothing(double smoothing)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxSmoothing);
  mDoubleSpinBoxSmoothing->setValue(smoothing);
}

void DtmInvDistWidgetImp::setRadius1(double radius1)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxRadius1);
  mDoubleSpinBoxRadius1->setValue(radius1);
}

void DtmInvDistWidgetImp::setRadius2(double radius2)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxRadius2);
  mDoubleSpinBoxRadius2->setValue(radius2);
}

void DtmInvDistWidgetImp::setAngle(double angle)
{
  const QSignalBlocker signalBlocker(mDoubleSpinBoxAngle);
  mDoubleSpinBoxAngle->setValue(angle);
}

void DtmInvDistWidgetImp::setMaxPoints(int maxPoints)
{
  const QSignalBlocker signalBlocker(mSpinBoxMaxPoints);
  mSpinBoxMaxPoints->setValue(maxPoints);
}

void DtmInvDistWidgetImp::setMinPoints(int minPoints)
{
  const QSignalBlocker signalBlocker(mSpinBoxMinPoints);
  mSpinBoxMinPoints->setValue(minPoints);
}

} // namespace graphos


