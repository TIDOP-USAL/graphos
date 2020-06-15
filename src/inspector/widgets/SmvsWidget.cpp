#include "SmvsWidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>

namespace inspector
{

SmvsWidgetImp::SmvsWidgetImp(QWidget *parent)
  : SmvsWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelInputImageScale(new QLabel(this)),
    mInputImageScale(new QSpinBox(this)),
    mLabelOutputDepthScale(new QLabel(this)),
    mOutputDepthScale(new QSpinBox(this)),
    mShadingBasedOptimization(new QCheckBox(this)),
    mSemiGlobalMatching(new QCheckBox(this)),
    mLabelSurfaceSmoothingFactor(new QLabel(this)),
    mSurfaceSmoothingFactor(new QDoubleSpinBox(this))
{
  this->initUI();
  this->initSignalAndSlots();
}

SmvsWidgetImp::~SmvsWidgetImp()
{

}

int SmvsWidgetImp::inputImageScale() const
{
  return mInputImageScale->value();
}

int SmvsWidgetImp::outputDepthScale() const
{
  return mOutputDepthScale->value();
}

bool SmvsWidgetImp::shadingBasedOptimization() const
{
  return mShadingBasedOptimization->isChecked();
}

bool SmvsWidgetImp::semiGlobalMatching() const
{
  return mSemiGlobalMatching->isChecked();
}

double SmvsWidgetImp::surfaceSmoothingFactor() const
{
  return mSurfaceSmoothingFactor->value();
}

void SmvsWidgetImp::setInputImageScale(int inputImageScale)
{
  const QSignalBlocker blockerInputImageScale(mInputImageScale);
  mInputImageScale->setValue(inputImageScale);
}

void SmvsWidgetImp::setOutputDepthScale(int outputDepthScale)
{
  const QSignalBlocker blockerOutputDepthScale(mOutputDepthScale);
  mOutputDepthScale->setValue(outputDepthScale);
}

void SmvsWidgetImp::setShadingBasedOptimization(bool shadingBasedOptimization)
{
  mShadingBasedOptimization->setChecked(shadingBasedOptimization);
}

void SmvsWidgetImp::setSemiGlobalMatching(bool semiGlobalMatching)
{
  mSemiGlobalMatching->setChecked(semiGlobalMatching);
}

void SmvsWidgetImp::setSurfaceSmoothingFactor(double surfaceSmoothingFactor)
{
  const QSignalBlocker blockerSurfaceSmoothingFactor(mSurfaceSmoothingFactor);
  mSurfaceSmoothingFactor->setValue(surfaceSmoothingFactor);
}


void SmvsWidgetImp::update()
{

}

void SmvsWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("SmvsWidgetImp", "Shading-Aware Multi-View Stereo Parameters", nullptr));
  mLabelInputImageScale->setText(QApplication::translate("SmvsWidgetImp", "Input Image Scale:"));
  mLabelOutputDepthScale->setText(QApplication::translate("SmvsWidgetImp", "Output Depth Scale:"));
  mShadingBasedOptimization->setText(QApplication::translate("SmvsWidgetImp", "Shading-Based Optimization:"));
  mSemiGlobalMatching->setText(QApplication::translate("SmvsWidgetImp", "Semi Global Matching:"));
  mLabelSurfaceSmoothingFactor->setText(QApplication::translate("SmvsWidgetImp", "Surface Smoothing Factor:"));
}

void SmvsWidgetImp::clear()
{
  const QSignalBlocker blockerInputImageScale(mInputImageScale);
  const QSignalBlocker blockerOutputDepthScale(mOutputDepthScale);
  const QSignalBlocker blockerSurfaceSmoothingFactor(mSurfaceSmoothingFactor);

  mInputImageScale->setValue(1);
  mOutputDepthScale->setValue(2);
  mShadingBasedOptimization->setChecked(false);
  mSemiGlobalMatching->setChecked(true);
  mSurfaceSmoothingFactor->setValue(1.0);
}

void SmvsWidgetImp::initUI()
{
  this->setWindowTitle("Shading-Aware Multi-View Stereo");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelInputImageScale, 0, 0);
  mInputImageScale->setRange(0, 6);
  propertiesLayout->addWidget(mInputImageScale, 0, 1);

  propertiesLayout->addWidget(mLabelOutputDepthScale, 1, 0);
  mOutputDepthScale->setRange(0, 6);
  propertiesLayout->addWidget(mOutputDepthScale, 1, 1);

  propertiesLayout->addWidget(mShadingBasedOptimization, 2, 0, 1, 2);

  propertiesLayout->addWidget(mSemiGlobalMatching, 3, 0, 1, 2);

  propertiesLayout->addWidget(mLabelSurfaceSmoothingFactor, 4, 0);
  mSurfaceSmoothingFactor->setRange(0., 10.);
  mSurfaceSmoothingFactor->setDecimals(2);
  mSurfaceSmoothingFactor->setSingleStep(0.1);
  propertiesLayout->addWidget(mSurfaceSmoothingFactor, 4, 1);

  this->retranslate();
  this->clear(); /// set default values
  this->update();
}

void SmvsWidgetImp::initSignalAndSlots()
{
  connect(mInputImageScale,          QOverload<int>::of(&QSpinBox::valueChanged),          this, &SmvsWidget::inputImageScaleChanged);
  connect(mOutputDepthScale,         QOverload<int>::of(&QSpinBox::valueChanged),          this, &SmvsWidget::outputDepthScaleChanged);
  connect(mShadingBasedOptimization, &QAbstractButton::clicked,                            this, &SmvsWidget::shadingBasedOptimizationChanged);
  connect(mSemiGlobalMatching,       &QAbstractButton::clicked,                            this, &SmvsWidget::semiGlobalMatchingChanged);
  connect(mSurfaceSmoothingFactor,   QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SmvsWidget::surfaceSmoothingFactorChanged);
}

} // namespace inspector
