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

#include "FeatureMatchingWidget.h"

TL_DISABLE_WARNINGS
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{


FeatureMatchingWidget::FeatureMatchingWidget(QWidget *parent)
  : GraphosWidgetView(parent),
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
    FeatureMatchingWidget::initUI();
    FeatureMatchingWidget::initSignalAndSlots();
}

FeatureMatchingWidget::~FeatureMatchingWidget()
{
}

void FeatureMatchingWidget::initUI()
{
    this->setObjectName(QStringLiteral("FeatureMatchingWidget"));
    this->setWindowTitle("Feature Matching Colmap");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *properties_layout = new QGridLayout();
    mGroupBox->setLayout(properties_layout);

    properties_layout->addWidget(mCheckBoxCrossCheck, 0, 0, 1, 1);

    properties_layout->addWidget(mLabelRatio, 1, 0, 1, 1);
    mDoubleSpinBoxRatio->setSingleStep(0.01);
    properties_layout->addWidget(mDoubleSpinBoxRatio, 1, 1, 1, 2);

    properties_layout->addWidget(mLabelDistance, 2, 0, 1, 1);
    mDoubleSpinBoxDistance->setSingleStep(0.01);
    properties_layout->addWidget(mDoubleSpinBoxDistance, 2, 1, 1, 2);

    properties_layout->addWidget(mLabelMaxError, 3, 0, 1, 1);
    mDoubleSpinBoxMaxError->setSingleStep(0.01);
    properties_layout->addWidget(mDoubleSpinBoxMaxError, 3, 1, 1, 2);

    properties_layout->addWidget(mLabelConfidence, 4, 0, 1, 1);
    mDoubleSpinBoxConfidence->setDecimals(4);
    mDoubleSpinBoxConfidence->setMaximum(1);
    mDoubleSpinBoxConfidence->setSingleStep(0.0001);
    properties_layout->addWidget(mDoubleSpinBoxConfidence, 4, 1, 1, 2);

    retranslate();
    clear();
    update();
}

void FeatureMatchingWidget::initSignalAndSlots()
{
    connect(mCheckBoxCrossCheck, &QAbstractButton::clicked, this, &FeatureMatchingWidget::crossCheckChanged);
    connect(mDoubleSpinBoxDistance, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::distanceChanged);
    connect(mDoubleSpinBoxRatio, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::ratioChanged);
    connect(mDoubleSpinBoxMaxError, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::maxErrorChanged);
    connect(mDoubleSpinBoxConfidence, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &FeatureMatchingWidget::confidenceChanged);
}

void FeatureMatchingWidget::clear()
{
    const QSignalBlocker blocker_ratio(mDoubleSpinBoxRatio);
    const QSignalBlocker blocker_distance(mDoubleSpinBoxDistance);
    const QSignalBlocker blocker_max_error(mDoubleSpinBoxMaxError);
    const QSignalBlocker blocker_confidence(mDoubleSpinBoxConfidence);

    mCheckBoxCrossCheck->setChecked(true);
    mDoubleSpinBoxRatio->setValue(0.8);
    mDoubleSpinBoxDistance->setValue(0.7);
    mDoubleSpinBoxMaxError->setValue(4.0);
    mDoubleSpinBoxConfidence->setValue(0.999);
}

void FeatureMatchingWidget::update()
{
}

void FeatureMatchingWidget::retranslate()
{
    mGroupBox->setTitle(QApplication::translate("FeatureMatchingWidget", "Feature Matching parameters", nullptr));
    mLabelMaxError->setText(QApplication::translate("FeatureMatchingWidget", "Max error", nullptr));
    mLabelDistance->setText(QApplication::translate("FeatureMatchingWidget", "Distance", nullptr));
    mCheckBoxCrossCheck->setText(QApplication::translate("FeatureMatchingWidget", "Cross Check", nullptr));
    mLabelConfidence->setText(QApplication::translate("FeatureMatchingWidget", "Confidence", nullptr));
    mLabelRatio->setText(QApplication::translate("FeatureMatchingWidget", "Ratio", nullptr));
}

auto FeatureMatchingWidget::crossCheck() const -> bool
{
    return mCheckBoxCrossCheck->isChecked();
}

auto FeatureMatchingWidget::ratio() const -> double
{
    return mDoubleSpinBoxRatio->value();
}

auto FeatureMatchingWidget::distance() const -> double
{
    return mDoubleSpinBoxDistance->value();
}

auto FeatureMatchingWidget::maxError() const -> double
{
    return mDoubleSpinBoxMaxError->value();
}

auto FeatureMatchingWidget::confidence() const -> double
{
    return mDoubleSpinBoxConfidence->value();
}

void FeatureMatchingWidget::enableCrossCheck(bool enable)
{
    mCheckBoxCrossCheck->setChecked(enable);
}

void FeatureMatchingWidget::setRatio(double ratio)
{
    const QSignalBlocker blocker_ratio(mDoubleSpinBoxRatio);
    mDoubleSpinBoxRatio->setValue(ratio);
}

void FeatureMatchingWidget::setDistance(double distance)
{
    const QSignalBlocker blocker_distance(mDoubleSpinBoxDistance);
    mDoubleSpinBoxDistance->setValue(distance);
}

//void FeatureMatchingWidget::setMaxMatches(int maxMatches)
//{
//}

void FeatureMatchingWidget::setMaxError(double error)
{
    const QSignalBlocker blocker_max_error(mDoubleSpinBoxMaxError);
    mDoubleSpinBoxMaxError->setValue(error);
}

void FeatureMatchingWidget::setConfidence(double confidence)
{
    const QSignalBlocker blocker_confidence(mDoubleSpinBoxConfidence);
    mDoubleSpinBoxConfidence->setValue(confidence);
}


} // namespace graphos
