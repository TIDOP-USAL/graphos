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

#include "SiftWidget.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QCheckBox>

namespace graphos
{

SiftWidget::SiftWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{}




SiftWidgetImp::SiftWidgetImp(QWidget *parent)
  : SiftWidget(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFeaturesNumber(new QLabel(this)),
    mFeaturesNumber(new QSpinBox(this)),
    mLabelOctaveLayers(new QLabel(this)),
    mOctaveLayers(new QSpinBox(this)),
    mContrastThresholdAuto(new QCheckBox(this)),
    mLabelContrastThreshold(new QLabel(this)),
    mContrastThreshold(new QDoubleSpinBox(this)),
    mLabelEdgeThreshold(new QLabel(this)),
    mEdgeThreshold(new QDoubleSpinBox(this)),
    mLabelSigma(new QLabel(this)),
    mSigma(new QDoubleSpinBox(this))
{
  SiftWidgetImp::initUI();
  SiftWidgetImp::initSignalAndSlots();
}

SiftWidgetImp::~SiftWidgetImp()
{

}

void SiftWidgetImp::initUI()
{
  this->setObjectName(QStringLiteral("SiftWidgetImp"));
  this->setWindowTitle("SIFT");

  QGridLayout *layout = new QGridLayout();
  layout->setContentsMargins(0,0,0,0);
  this->setLayout(layout);

  layout->addWidget(mGroupBox);

  QGridLayout *propertiesLayout = new QGridLayout();
  mGroupBox->setLayout(propertiesLayout);

  propertiesLayout->addWidget(mLabelFeaturesNumber, 0, 0);
  mFeaturesNumber->setRange(0, 50000);
  propertiesLayout->addWidget(mFeaturesNumber, 0, 1);

  propertiesLayout->addWidget(mLabelOctaveLayers, 1, 0);
  mOctaveLayers->setRange(0, 10);
  propertiesLayout->addWidget(mOctaveLayers, 1, 1);

  propertiesLayout->addWidget(mContrastThresholdAuto, 2, 1);

  propertiesLayout->addWidget(mLabelContrastThreshold, 3, 0);
  mContrastThreshold->setRange(0., 10.);  //TODO: ver que el rango tenga sentido
  mContrastThreshold->setDecimals(4);
  mContrastThreshold->setSingleStep(0.001);
  propertiesLayout->addWidget(mContrastThreshold, 3, 1);

  propertiesLayout->addWidget(mLabelEdgeThreshold, 4, 0);
  mEdgeThreshold->setRange(0., 100.);
  propertiesLayout->addWidget(mEdgeThreshold, 4, 1);

  propertiesLayout->addWidget(mLabelSigma, 5, 0);
  mSigma->setRange(0., 100.);
  mSigma->setSingleStep(0.1);
  propertiesLayout->addWidget(mSigma, 5, 1);

  SiftWidgetImp::retranslate();
  SiftWidgetImp::clear(); /// set default values
  SiftWidgetImp::update();
}

void SiftWidgetImp::initSignalAndSlots()
{
  connect(mFeaturesNumber,         SIGNAL(valueChanged(int)),    this, SIGNAL(featuresNumberChange(int)));
  connect(mOctaveLayers,           SIGNAL(valueChanged(int)),    this, SIGNAL(octaveLayersChange(int)));
  connect(mContrastThreshold,      SIGNAL(valueChanged(double)), this, SIGNAL(contrastThresholdChange(double)));
  connect(mContrastThresholdAuto,  SIGNAL(clicked(bool)),        this, SIGNAL(contrastThresholdAutoChange(bool)));
  connect(mEdgeThreshold,          SIGNAL(valueChanged(double)), this, SIGNAL(edgeThresholdChange(double)));
  connect(mSigma,                  SIGNAL(valueChanged(double)), this, SIGNAL(sigmaChange(double)));

  connect(mContrastThresholdAuto,  SIGNAL(clicked(bool)),        mContrastThreshold, SLOT(setDisabled(bool)));
}

void SiftWidgetImp::clear()
{
  const QSignalBlocker blockerFeaturesNumber(mFeaturesNumber);
  const QSignalBlocker blockerOctaveLayers(mOctaveLayers);
  const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
  const QSignalBlocker blockerEdgeThreshold(mEdgeThreshold);
  const QSignalBlocker blockerSigma(mSigma);

  mFeaturesNumber->setValue(5000);
  mOctaveLayers->setValue(3);
  mContrastThresholdAuto->setChecked(true);
  mContrastThreshold->setValue(0.02/3.);
  mEdgeThreshold->setValue(10.);
  mSigma->setValue(1.6);
}

void SiftWidgetImp::update()
{
  if (mContrastThresholdAuto) {
    mContrastThreshold->setValue(0.02 / static_cast<double>(mOctaveLayers->value()));
    mContrastThreshold->setDisabled(true);
  } else {
    mContrastThreshold->setDisabled(false);
  }
}

void SiftWidgetImp::retranslate()
{
  mGroupBox->setTitle(QApplication::translate("SiftWidget", "SIFT Parameters", nullptr));
  mLabelFeaturesNumber->setText(QApplication::translate("SiftWidget", "Features Number:"));
  mLabelOctaveLayers->setText(QApplication::translate("SiftWidget", "Octave Layers:"));
  mContrastThresholdAuto->setText(QApplication::translate("SiftWidget", "Contrast Threshold Auto"));
  mLabelContrastThreshold->setText(QApplication::translate("SiftWidget", "Contrast Threshold:"));
  mLabelEdgeThreshold->setText(QApplication::translate("SiftWidget", "Edge Threshold:"));
  mLabelSigma->setText(QApplication::translate("SiftWidget", "Sigma:"));
}

int SiftWidgetImp::featuresNumber() const
{
  return mFeaturesNumber->value();
}

int SiftWidgetImp::octaveLayers() const
{
  return mOctaveLayers->value();
}

double SiftWidgetImp::contrastThreshold() const 
{
  return mContrastThreshold->value();
}

bool SiftWidgetImp::constrastThresholdAuto() const
{
  return mContrastThresholdAuto->isChecked();
}

double SiftWidgetImp::edgeThreshold() const
{
  return mEdgeThreshold->value();
}

double SiftWidgetImp::sigma() const
{
  return mSigma->value();
}

void SiftWidgetImp::setFeaturesNumber(int featuresNumber)
{
  const QSignalBlocker blockerFeaturesNumber(mFeaturesNumber);
  mFeaturesNumber->setValue(featuresNumber);
}

void SiftWidgetImp::setOctaveLayers(int octaveLayers)
{
  const QSignalBlocker blockerFeaturesNumber(mOctaveLayers);
  mOctaveLayers->setValue(octaveLayers);
  if (mContrastThresholdAuto->isChecked()) {
    const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
    mContrastThreshold->setValue(0.02 / static_cast<double>(mOctaveLayers->value()));
  }
}

void SiftWidgetImp::setContrastThreshold(double contrastThreshold)
{
  if (!mContrastThresholdAuto->isChecked()) {
    const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
    mContrastThreshold->setValue(contrastThreshold);
  }
}

void SiftWidgetImp::setContrastThresholdAuto(bool active)
{
  mContrastThresholdAuto->setChecked(active);
  const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
  mContrastThreshold->setValue(0.02 / static_cast<double>(mOctaveLayers->value()));
}

void SiftWidgetImp::setEdgeThreshold(double edgeThreshold)
{
  const QSignalBlocker blockerEdgeThreshold(mEdgeThreshold);
  mEdgeThreshold->setValue(edgeThreshold);
}

void SiftWidgetImp::setSigma(double sigma)
{
  const QSignalBlocker blockerSigma(mSigma);
  mSigma->setValue(sigma);
}


} // namespace photomatch
