#include "FeatureMatchingWidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QApplication>

namespace graphos
{

FeatureMatchingWidgetImp::FeatureMatchingWidgetImp(QWidget *parent)
  : FeatureMatchingWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mCheckBoxCrossCheck(new QCheckBox(this)),
    mLabelRatio(new QLabel(this)),
    mDoubleSpinBoxRatio(new QDoubleSpinBox(this)),
    mLabelDistance(new QLabel(this)),
    mDoubleSpinBoxDistance(new QDoubleSpinBox(this)),
    mLabelMaxError(new QLabel(this)),
    mDoubleSpinBoxMaxError(new QDoubleSpinBox(this)),
    mLabelConfidence(new QLabel(this)),
    mDoubleSpinBoxConfidence(new QDoubleSpinBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

FeatureMatchingWidgetImp::~FeatureMatchingWidgetImp()
{
}

void FeatureMatchingWidgetImp::initUI()
{
  this->setObjectName(QStringLiteral("FeatureMatchingWidget"));
  this->setWindowTitle("Feature Matching Colmap");
  //this->resize(439, 371);

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mCheckBoxCrossCheck, 0, 0, 1, 1);

  propertiesLayout->addWidget(mLabelRatio, 1, 0, 1, 1);
  mDoubleSpinBoxRatio->setSingleStep(0.01);
  propertiesLayout->addWidget(mDoubleSpinBoxRatio, 1, 1, 1, 2);

  propertiesLayout->addWidget(mLabelDistance, 2, 0, 1, 1);
  mDoubleSpinBoxDistance->setSingleStep(0.01);
  propertiesLayout->addWidget(mDoubleSpinBoxDistance, 2, 1, 1, 2);

  propertiesLayout->addWidget(mLabelMaxError, 3, 0, 1, 1);
  mDoubleSpinBoxMaxError->setSingleStep(0.01);
  propertiesLayout->addWidget(mDoubleSpinBoxMaxError, 3, 1, 1, 2);

  propertiesLayout->addWidget(mLabelConfidence, 4, 0, 1, 1);
  mDoubleSpinBoxConfidence->setDecimals(4);
  mDoubleSpinBoxConfidence->setMaximum(1);
  mDoubleSpinBoxConfidence->setSingleStep(0.0001);
  propertiesLayout->addWidget(mDoubleSpinBoxConfidence, 4, 1, 1, 2);

  this->retranslate();
  this->clear();
  this->update();
}

void FeatureMatchingWidgetImp::initSignalAndSlots()
{
  connect(mCheckBoxCrossCheck,      &QAbstractButton::clicked,                            this, &FeatureMatchingWidget::crossCheckChanged);
  connect(mDoubleSpinBoxDistance,   QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::distanceChanged);
  connect(mDoubleSpinBoxRatio,      QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::ratioChanged);
  connect(mDoubleSpinBoxMaxError,   QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::maxErrorChanged);
  connect(mDoubleSpinBoxConfidence, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::confidenceChanged);
}

void FeatureMatchingWidgetImp::clear()
{
  const QSignalBlocker blockerRatio(mDoubleSpinBoxRatio);
  const QSignalBlocker blockerDistance(mDoubleSpinBoxDistance);
  const QSignalBlocker blockerMaxError(mDoubleSpinBoxMaxError);
  const QSignalBlocker blockerConfidence(mDoubleSpinBoxConfidence);

  mCheckBoxCrossCheck->setChecked(true);
  mDoubleSpinBoxRatio->setValue(0.8);
  mDoubleSpinBoxDistance->setValue(0.7);
  mDoubleSpinBoxMaxError->setValue(4.0);
  mDoubleSpinBoxConfidence->setValue(0.999);
}

void FeatureMatchingWidgetImp::update()
{
}

void FeatureMatchingWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("FeatureMatchingWidget", "Feature Matching parameters", nullptr));
  mLabelMaxError->setText(QApplication::translate("FeatureMatchingWidget", "Max error", nullptr));
  mLabelDistance->setText(QApplication::translate("FeatureMatchingWidget", "Distance", nullptr));
  mCheckBoxCrossCheck->setText(QApplication::translate("FeatureMatchingWidget", "Cross Check", nullptr));
  mLabelConfidence->setText(QApplication::translate("FeatureMatchingWidget", "Confidence", nullptr));
  mLabelRatio->setText(QApplication::translate("FeatureMatchingWidget", "Ratio", nullptr));
}

bool FeatureMatchingWidgetImp::crossCheck() const
{
  return mCheckBoxCrossCheck->isChecked();
}

double FeatureMatchingWidgetImp::ratio() const
{
  return mDoubleSpinBoxRatio->value();
}

double FeatureMatchingWidgetImp::distance() const
{
  return mDoubleSpinBoxDistance->value();
}

//int FeatureMatchingWidgetImp::maxMatches() const
//{
//}

double FeatureMatchingWidgetImp::maxError() const
{
  return mDoubleSpinBoxMaxError->value();
}

double FeatureMatchingWidgetImp::confidence() const
{
  return mDoubleSpinBoxConfidence->value();
}

void FeatureMatchingWidgetImp::enableCrossCheck(bool enable)
{
  mCheckBoxCrossCheck->setChecked(enable);
}

void FeatureMatchingWidgetImp::setRatio(double ratio)
{
  const QSignalBlocker blockerRatio(mDoubleSpinBoxRatio);
  mDoubleSpinBoxRatio->setValue(ratio);
}

void FeatureMatchingWidgetImp::setDistance(double distance)
{
  const QSignalBlocker blockerDistance(mDoubleSpinBoxDistance);
  mDoubleSpinBoxDistance->setValue(distance);
}

//void FeatureMatchingWidgetImp::setMaxMatches(int maxMatches)
//{
//}

void FeatureMatchingWidgetImp::setMaxError(double error)
{
  const QSignalBlocker blockerMaxError(mDoubleSpinBoxMaxError);
  mDoubleSpinBoxMaxError->setValue(error);
}

void FeatureMatchingWidgetImp::setConfidence(double confidence)
{
  const QSignalBlocker blockerConfidence(mDoubleSpinBoxConfidence);
  mDoubleSpinBoxConfidence->setValue(confidence);
}

} // namespace graphos
