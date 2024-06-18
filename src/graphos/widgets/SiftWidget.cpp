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

#include "SiftWidget.h"

TL_DISABLE_WARNINGS
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QCheckBox>
TL_DEFAULT_WARNINGS

namespace graphos
{



SiftWidget::SiftWidget(QWidget *parent)
  : GraphosWidgetView(parent),
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
    mDomainSizePooling(new QCheckBox(this))
{
    SiftWidget::initUI();
    SiftWidget::initSignalAndSlots();
}

SiftWidget::~SiftWidget()
{

}

void SiftWidget::initUI()
{
    this->setObjectName(QStringLiteral("SiftWidget"));
    this->setWindowTitle("SIFT");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
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

    propertiesLayout->addWidget(mDomainSizePooling, 5, 0, 1, 2);

    SiftWidget::retranslate();
    SiftWidget::clear(); /// set default values
    SiftWidget::update();
}

void SiftWidget::initSignalAndSlots()
{
    connect(mFeaturesNumber, SIGNAL(valueChanged(int)), this, SIGNAL(featuresNumberChange(int)));
    connect(mOctaveLayers, SIGNAL(valueChanged(int)), this, SIGNAL(octaveLayersChange(int)));
    connect(mContrastThreshold, SIGNAL(valueChanged(double)), this, SIGNAL(contrastThresholdChange(double)));
    connect(mContrastThresholdAuto, SIGNAL(clicked(bool)), this, SIGNAL(contrastThresholdAutoChange(bool)));
    connect(mEdgeThreshold, SIGNAL(valueChanged(double)), this, SIGNAL(edgeThresholdChange(double)));
    connect(mContrastThresholdAuto, SIGNAL(clicked(bool)), mContrastThreshold, SLOT(setDisabled(bool)));
    connect(mDomainSizePooling, SIGNAL(clicked(bool)), this, SIGNAL(domainSizePooling(bool)));
}

void SiftWidget::clear()
{
    const QSignalBlocker blocker_features_number(mFeaturesNumber);
    const QSignalBlocker blocker_octave_layers(mOctaveLayers);
    const QSignalBlocker blocker_contrast_threshold(mContrastThreshold);
    const QSignalBlocker blocker_edge_threshold(mEdgeThreshold);

    mFeaturesNumber->setValue(5000);
    mOctaveLayers->setValue(3);
    mContrastThresholdAuto->setChecked(true);
    mContrastThreshold->setValue(0.02 / 3.);
    mEdgeThreshold->setValue(10.);
    mDomainSizePooling->setChecked(true);
}

void SiftWidget::update()
{
    if (mContrastThresholdAuto) {
        mContrastThreshold->setValue(0.02 / static_cast<double>(mOctaveLayers->value()));
        mContrastThreshold->setDisabled(true);
    } else {
        mContrastThreshold->setDisabled(false);
    }
}

void SiftWidget::retranslate()
{
    mGroupBox->setTitle(QApplication::translate("SiftWidget", "SIFT Parameters", nullptr));
    mLabelFeaturesNumber->setText(QApplication::translate("SiftWidget", "Features Number:"));
    mLabelOctaveLayers->setText(QApplication::translate("SiftWidget", "Octave Layers:"));
    mContrastThresholdAuto->setText(QApplication::translate("SiftWidget", "Contrast Threshold Auto"));
    mLabelContrastThreshold->setText(QApplication::translate("SiftWidget", "Contrast Threshold:"));
    mLabelEdgeThreshold->setText(QApplication::translate("SiftWidget", "Edge Threshold:"));
    mDomainSizePooling->setText(QApplication::translate("SiftWidget", "Domain Size Pooling"));
}

auto SiftWidget::featuresNumber() const -> int
{
    return mFeaturesNumber->value();
}

auto SiftWidget::octaveLayers() const -> int
{
    return mOctaveLayers->value();
}

auto SiftWidget::contrastThreshold() const -> double
{
    return mContrastThreshold->value();
}

auto SiftWidget::constrastThresholdAuto() const -> bool
{
    return mContrastThresholdAuto->isChecked();
}

auto SiftWidget::edgeThreshold() const -> double
{
    return mEdgeThreshold->value();
}

auto SiftWidget::domainSizePooling() const -> bool
{
    return mDomainSizePooling->isChecked();
}

void SiftWidget::setFeaturesNumber(int featuresNumber)
{
    const QSignalBlocker blockerFeaturesNumber(mFeaturesNumber);
    mFeaturesNumber->setValue(featuresNumber);
}

void SiftWidget::setOctaveLayers(int octaveLayers)
{
    const QSignalBlocker blockerFeaturesNumber(mOctaveLayers);
    mOctaveLayers->setValue(octaveLayers);
    if (mContrastThresholdAuto->isChecked()) {
        const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
        mContrastThreshold->setValue(0.02 / static_cast<double>(mOctaveLayers->value()));
    }
}

void SiftWidget::setContrastThreshold(double contrastThreshold)
{
    if (!mContrastThresholdAuto->isChecked()) {
        const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
        mContrastThreshold->setValue(contrastThreshold);
    }
}

void SiftWidget::setContrastThresholdAuto(bool active)
{
    mContrastThresholdAuto->setChecked(active);
    const QSignalBlocker blockerContrastThreshold(mContrastThreshold);
    mContrastThreshold->setValue(0.02 / static_cast<double>(mOctaveLayers->value()));
}

void SiftWidget::setEdgeThreshold(double edgeThreshold)
{
    const QSignalBlocker blockerEdgeThreshold(mEdgeThreshold);
    mEdgeThreshold->setValue(edgeThreshold);
}

void SiftWidget::setDomainSizePooling(bool domainSizePooling)
{
    mDomainSizePooling->setChecked(domainSizePooling);
}


} // namespace photomatch
