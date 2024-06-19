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

#include "SmvsWidget.h"

TL_DISABLE_WARNINGS
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{



SmvsWidget::SmvsWidget(QWidget *parent)
  : GraphosWidgetView(parent),
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
    SmvsWidget::initUI();
    SmvsWidget::initSignalAndSlots();
}

SmvsWidget::~SmvsWidget() = default;

void SmvsWidget::initUI()
{
    this->setWindowTitle("Shading-Aware Multi-View Stereo");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *properties_layout = new QGridLayout();
    mGroupBox->setLayout(properties_layout);

    properties_layout->addWidget(mLabelInputImageScale, 0, 0);
    mInputImageScale->setRange(0, 6);
    properties_layout->addWidget(mInputImageScale, 0, 1);

    properties_layout->addWidget(mLabelOutputDepthScale, 1, 0);
    mOutputDepthScale->setRange(0, 6);
    properties_layout->addWidget(mOutputDepthScale, 1, 1);

    properties_layout->addWidget(mShadingBasedOptimization, 2, 0, 1, 2);

    properties_layout->addWidget(mSemiGlobalMatching, 3, 0, 1, 2);

    properties_layout->addWidget(mLabelSurfaceSmoothingFactor, 4, 0);
    mSurfaceSmoothingFactor->setRange(0., 10.);
    mSurfaceSmoothingFactor->setDecimals(2);
    mSurfaceSmoothingFactor->setSingleStep(0.1);
    properties_layout->addWidget(mSurfaceSmoothingFactor, 4, 1);

    retranslate();
    clear(); /// set default values
    update();
}

void SmvsWidget::initSignalAndSlots()
{
    connect(mInputImageScale, QOverload<int>::of(&QSpinBox::valueChanged), this, &SmvsWidget::inputImageScaleChanged);
    connect(mOutputDepthScale, QOverload<int>::of(&QSpinBox::valueChanged), this, &SmvsWidget::outputDepthScaleChanged);
    connect(mShadingBasedOptimization, &QAbstractButton::clicked, this, &SmvsWidget::shadingBasedOptimizationChanged);
    connect(mSemiGlobalMatching, &QAbstractButton::clicked, this, &SmvsWidget::semiGlobalMatchingChanged);
    connect(mSurfaceSmoothingFactor, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SmvsWidget::surfaceSmoothingFactorChanged);
}

void SmvsWidget::clear()
{
    const QSignalBlocker blocker_input_image_scale(mInputImageScale);
    const QSignalBlocker blocker_output_depth_scale(mOutputDepthScale);
    const QSignalBlocker blocker_surface_smoothing_factor(mSurfaceSmoothingFactor);

    mInputImageScale->setValue(1);
    mOutputDepthScale->setValue(2);
    mShadingBasedOptimization->setChecked(false);
    mSemiGlobalMatching->setChecked(true);
    mSurfaceSmoothingFactor->setValue(1.0);
}

void SmvsWidget::update()
{

}

void SmvsWidget::retranslate()
{
    mGroupBox->setTitle(QApplication::translate("SmvsWidget", "Shading-Aware Multi-View Stereo Parameters", nullptr));
    mLabelInputImageScale->setText(QApplication::translate("SmvsWidget", "Image Scale:"));
    mLabelOutputDepthScale->setText(QApplication::translate("SmvsWidget", "Depth Scale:"));
    mShadingBasedOptimization->setText(QApplication::translate("SmvsWidget", "Shading-Based Optimization:"));
    mSemiGlobalMatching->setText(QApplication::translate("SmvsWidget", "Semi Global Matching:"));
    mLabelSurfaceSmoothingFactor->setText(QApplication::translate("SmvsWidget", "Surface Smoothing Factor:"));
}

auto SmvsWidget::inputImageScale() const -> int
{
    return mInputImageScale->value();
}

auto SmvsWidget::outputDepthScale() const -> int
{
    return mOutputDepthScale->value();
}

auto SmvsWidget::shadingBasedOptimization() const -> bool
{
    return mShadingBasedOptimization->isChecked();
}

auto SmvsWidget::semiGlobalMatching() const -> bool
{
    return mSemiGlobalMatching->isChecked();
}

auto SmvsWidget::surfaceSmoothingFactor() const -> double
{
    return mSurfaceSmoothingFactor->value();
}

void SmvsWidget::setInputImageScale(int inputImageScale)
{
    const QSignalBlocker blockerInputImageScale(mInputImageScale);
    mInputImageScale->setValue(inputImageScale);
}

void SmvsWidget::setOutputDepthScale(int outputDepthScale)
{
    const QSignalBlocker blockerOutputDepthScale(mOutputDepthScale);
    mOutputDepthScale->setValue(outputDepthScale);
}

void SmvsWidget::setShadingBasedOptimization(bool shadingBasedOptimization)
{
    mShadingBasedOptimization->setChecked(shadingBasedOptimization);
}

void SmvsWidget::setSemiGlobalMatching(bool semiGlobalMatching)
{
    mSemiGlobalMatching->setChecked(semiGlobalMatching);
}

void SmvsWidget::setSurfaceSmoothingFactor(double surfaceSmoothingFactor)
{
    const QSignalBlocker blockerSurfaceSmoothingFactor(mSurfaceSmoothingFactor);
    mSurfaceSmoothingFactor->setValue(surfaceSmoothingFactor);
}


} // namespace graphos
