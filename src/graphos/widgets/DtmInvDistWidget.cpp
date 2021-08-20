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
  this->initUI();
  this->initSignalAndSlots();
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

  this->retranslate();
  this->clear(); /// set default values
  this->update();
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


