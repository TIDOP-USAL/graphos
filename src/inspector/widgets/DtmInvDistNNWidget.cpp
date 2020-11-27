#include "DtmInvDistNNWidget.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>

namespace inspector
{

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
  this->initUI();
  this->initSignalAndSlots();
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
  propertiesLayout->addWidget(mDoubleSpinBoxPower, 0, 1);

  propertiesLayout->addWidget(mLabelSmoothing, 1, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxSmoothing, 1, 1);

  propertiesLayout->addWidget(mLabelRadius, 2, 0);
  propertiesLayout->addWidget(mDoubleSpinBoxRadius, 2, 1);

  propertiesLayout->addWidget(mLabelMaxPoints, 3, 0);
  propertiesLayout->addWidget(mSpinBoxMaxPoints, 3, 1);

  propertiesLayout->addWidget(mLabelMinPoints, 4, 0);
  propertiesLayout->addWidget(mSpinBoxMinPoints, 4, 1);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
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
  mDoubleSpinBoxRadius->setValue(0.0);
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

} // namespace inspector


