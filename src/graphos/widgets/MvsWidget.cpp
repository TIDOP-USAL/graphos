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

#include "MvsWidget.h"

TL_DISABLE_WARNINGS
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
TL_DEFAULT_WARNINGS

namespace graphos
{

MvsWidget::MvsWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelQuality(new QLabel(this)),
    mComboBoxQuality(new QComboBox(this)),
    //mLabelResolutionLevel(new QLabel(this)),
    //mSpinBoxResolutionLevel(new QSpinBox(this)),
    //mLabelMinResolution(new QLabel(this)),
    //mSpinBoxMinResolution(new QSpinBox(this)),
    //mLabelMaxResolution(new QLabel(this)),
    //mSpinBoxMaxResolution(new QSpinBox(this)),
    mLabelNumberViews(new QLabel(this)),
    mSpinBoxNumberViews(new QSpinBox(this)),
    mLabelNumberViewsFuse(new QLabel(this)),
    mSpinBoxNumberViewsFuse(new QSpinBox(this)),
    mCheckBoxEstimateColors(new QCheckBox(this)),
    mCheckBoxEstimateNormals(new QCheckBox(this))
{
    MvsWidget::initUI();
    MvsWidget::initSignalAndSlots();
}

MvsWidget::~MvsWidget()
{

}

auto MvsWidget::quality() const -> int
{
    return mComboBoxQuality->currentIndex();
}

//int MvsWidget::resolutionLevel() const
//{
//    return mSpinBoxResolutionLevel->value();
//}
//
//int MvsWidget::minResolution() const
//{
//    return mSpinBoxMinResolution->value();
//}
//
//int MvsWidget::maxResolution() const
//{
//    return mSpinBoxMaxResolution->value();
//}

auto MvsWidget::numberViews() const -> int
{
    return mSpinBoxNumberViews->value();
}

auto MvsWidget::numberViewsFuse() const -> int
{
    return mSpinBoxNumberViewsFuse->value();
}

auto MvsWidget::estimateColors() const -> bool
{
    return mCheckBoxEstimateColors->isChecked();
}

auto MvsWidget::estimateNormals() const -> bool
{
    return mCheckBoxEstimateNormals->isChecked();
}

void MvsWidget::setQuality(int quality)
{
    const QSignalBlocker blocker(mComboBoxQuality);
    mComboBoxQuality->setCurrentIndex(quality);
}

//void MvsWidget::setResolutionLevel(int resolutionLevel)
//{
//    const QSignalBlocker blockerResolutionLevel(mSpinBoxResolutionLevel);
//    mSpinBoxResolutionLevel->setValue(resolutionLevel);
//}
//
//void MvsWidget::setMinResolution(int minResolution)
//{
//    const QSignalBlocker blockerMinResolution(mSpinBoxMinResolution);
//    mSpinBoxMinResolution->setValue(minResolution);
//}
//
//void MvsWidget::setMaxResolution(int maxResolution)
//{
//    const QSignalBlocker blockerMaxResolution(mSpinBoxMaxResolution);
//    mSpinBoxMaxResolution->setValue(maxResolution);
//}

void MvsWidget::setNumberViews(int numberViews)
{
    const QSignalBlocker blocker_number_views(mSpinBoxNumberViews);
    mSpinBoxNumberViews->setValue(numberViews);
}

void MvsWidget::setNumberViewsFuse(int numberViewsFuse)
{
    const QSignalBlocker blocker_number_views_fuse(mSpinBoxNumberViewsFuse);
    mSpinBoxNumberViewsFuse->setValue(numberViewsFuse);
}

void MvsWidget::setEstimateColors(bool estimateColors)
{
    mCheckBoxEstimateColors->setChecked(estimateColors);
}

void MvsWidget::setEstimateNormals(bool estimateNormals)
{
    mCheckBoxEstimateNormals->setChecked(estimateNormals);
}

void MvsWidget::update()
{

}

void MvsWidget::retranslate()
{
    mGroupBox->setTitle(QApplication::translate("MvsWidget", "MVS", nullptr));
    //mLabelResolutionLevel->setText(QApplication::translate("MvsWidget", "Resolution Level:"));
    //mLabelMinResolution->setText(QApplication::translate("MvsWidget", "Min Resolution:"));
    //mLabelMaxResolution->setText(QApplication::translate("MvsWidget", "Max Resolution:"));
    mLabelQuality->setText(QApplication::translate("MvsWidget", "Quality"));
    mComboBoxQuality->setItemText(0, QApplication::translate("MvsWidget", "Ultra"));
    mComboBoxQuality->setItemText(1, QApplication::translate("MvsWidget", "High"));
    mComboBoxQuality->setItemText(2, QApplication::translate("MvsWidget", "Medium"));
    mComboBoxQuality->setItemText(3, QApplication::translate("MvsWidget", "Low"));
    mLabelNumberViews->setText(QApplication::translate("MvsWidget", "Number Views:"));
    mLabelNumberViewsFuse->setText(QApplication::translate("MvsWidget", "Number Views Fuse:"));
    mCheckBoxEstimateColors->setText(QApplication::translate("MvsWidget", "Estimate colors"));
    mCheckBoxEstimateNormals->setText(QApplication::translate("MvsWidget", "Estimate normals"));
}

void MvsWidget::clear()
{
    //const QSignalBlocker blockerBoxResolutionLevel(mSpinBoxResolutionLevel);
    //const QSignalBlocker blockerMinResolution(mSpinBoxMinResolution);
    //const QSignalBlocker blockerMaxResolution(mSpinBoxMaxResolution);
    const QSignalBlocker blocker_number_views(mSpinBoxNumberViews);
    const QSignalBlocker blocker_number_views_fuse(mSpinBoxNumberViewsFuse);
    const QSignalBlocker blocker_quality(mComboBoxQuality);

    mComboBoxQuality->setCurrentIndex(2);
    //mSpinBoxResolutionLevel->setValue(1);
    //mSpinBoxMinResolution->setValue(640);
    //mSpinBoxMaxResolution->setValue(3200);
    mSpinBoxNumberViews->setValue(5);
    mSpinBoxNumberViewsFuse->setValue(3);
    mCheckBoxEstimateColors->setChecked(true);
    mCheckBoxEstimateNormals->setChecked(true);
}

void MvsWidget::initUI()
{
    this->setWindowTitle("MVS");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *properties_layout = new QGridLayout();
    mGroupBox->setLayout(properties_layout);

    
    properties_layout->addWidget(mLabelQuality, 1, 0);
    mComboBoxQuality->addItem("");
    mComboBoxQuality->addItem("");
    mComboBoxQuality->addItem("");
    mComboBoxQuality->addItem("");
    mComboBoxQuality->setCurrentIndex(1);
    properties_layout->addWidget(mComboBoxQuality, 1, 1);

    //propertiesLayout->addWidget(mLabelResolutionLevel, 0, 0);
    //mSpinBoxResolutionLevel->setRange(1, 4);
    //propertiesLayout->addWidget(mSpinBoxResolutionLevel, 0, 1);

    //propertiesLayout->addWidget(mLabelMinResolution, 1, 0);
    //mSpinBoxMinResolution->setRange(0, 10000);
    //propertiesLayout->addWidget(mSpinBoxMinResolution, 1, 1);

    //propertiesLayout->addWidget(mLabelMaxResolution, 2, 0);
    //mSpinBoxMaxResolution->setRange(1, 10000);
    //propertiesLayout->addWidget(mSpinBoxMaxResolution, 2, 1);

    properties_layout->addWidget(mLabelNumberViews, 2, 0);
    mSpinBoxNumberViews->setRange(1, 100);
    properties_layout->addWidget(mSpinBoxNumberViews, 2, 1);

    properties_layout->addWidget(mLabelNumberViewsFuse, 3, 0);
    mSpinBoxNumberViewsFuse->setRange(1, 100);
    properties_layout->addWidget(mSpinBoxNumberViewsFuse, 3, 1);

    properties_layout->addWidget(mCheckBoxEstimateColors, 4, 0, 1, 2);
    properties_layout->addWidget(mCheckBoxEstimateNormals, 5, 0, 1, 2);

    MvsWidget::retranslate();
    MvsWidget::clear(); /// set default values
    MvsWidget::update();
}

void MvsWidget::initSignalAndSlots()
{
    connect(mComboBoxQuality, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MvsWidget::qualityChanged);
    //connect(mSpinBoxResolutionLevel, QOverload<int>::of(&QSpinBox::valueChanged), this, &MvsWidget::resolutionLevelChanged);
    //connect(mSpinBoxMinResolution, QOverload<int>::of(&QSpinBox::valueChanged), this, &MvsWidget::minResolutionChanged);
    //connect(mSpinBoxMaxResolution, QOverload<int>::of(&QSpinBox::valueChanged), this, &MvsWidget::maxResolutionChanged);
    connect(mSpinBoxNumberViewsFuse, QOverload<int>::of(&QSpinBox::valueChanged), this, &MvsWidget::numberViewsFuseChanged);
    connect(mSpinBoxNumberViewsFuse, QOverload<int>::of(&QSpinBox::valueChanged), this, &MvsWidget::numberViewsFuseChanged);
}


} // namespace graphos
