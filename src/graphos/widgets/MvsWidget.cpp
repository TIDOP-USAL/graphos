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
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QComboBox>
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
    mSpinBoxNumberViewsFuse(new QSpinBox(this))
{
    MvsWidget::initUI();
    MvsWidget::initSignalAndSlots();
}

MvsWidget::~MvsWidget()
{

}

int MvsWidget::quality() const
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

int MvsWidget::numberViews() const
{
    return mSpinBoxNumberViews->value();
}

int MvsWidget::numberViewsFuse() const
{
    return mSpinBoxNumberViewsFuse->value();
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
    const QSignalBlocker blockerNumberViews(mSpinBoxNumberViews);
    mSpinBoxNumberViews->setValue(numberViews);
}

void MvsWidget::setNumberViewsFuse(int numberViewsFuse)
{
    const QSignalBlocker blockerNumberViewsFuse(mSpinBoxNumberViewsFuse);
    mSpinBoxNumberViewsFuse->setValue(numberViewsFuse);
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
}

void MvsWidget::clear()
{
    //const QSignalBlocker blockerBoxResolutionLevel(mSpinBoxResolutionLevel);
    //const QSignalBlocker blockerMinResolution(mSpinBoxMinResolution);
    //const QSignalBlocker blockerMaxResolution(mSpinBoxMaxResolution);
    const QSignalBlocker blockerNumberViews(mSpinBoxNumberViews);
    const QSignalBlocker blockerNumberViewsFuse(mSpinBoxNumberViewsFuse);
    const QSignalBlocker blockerQuality(mComboBoxQuality);

    mComboBoxQuality->setCurrentIndex(2);
    //mSpinBoxResolutionLevel->setValue(1);
    //mSpinBoxMinResolution->setValue(640);
    //mSpinBoxMaxResolution->setValue(3200);
    mSpinBoxNumberViews->setValue(5);
    mSpinBoxNumberViewsFuse->setValue(3);
}

void MvsWidget::initUI()
{
    this->setWindowTitle("MVS");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *propertiesLayout = new QGridLayout();
    mGroupBox->setLayout(propertiesLayout);

    
    propertiesLayout->addWidget(mLabelQuality, 1, 0);
    mComboBoxQuality->addItem("");
    mComboBoxQuality->addItem("");
    mComboBoxQuality->addItem("");
    mComboBoxQuality->addItem("");
    mComboBoxQuality->setCurrentIndex(1);
    propertiesLayout->addWidget(mComboBoxQuality, 1, 1);

    //propertiesLayout->addWidget(mLabelResolutionLevel, 0, 0);
    //mSpinBoxResolutionLevel->setRange(1, 4);
    //propertiesLayout->addWidget(mSpinBoxResolutionLevel, 0, 1);

    //propertiesLayout->addWidget(mLabelMinResolution, 1, 0);
    //mSpinBoxMinResolution->setRange(0, 10000);
    //propertiesLayout->addWidget(mSpinBoxMinResolution, 1, 1);

    //propertiesLayout->addWidget(mLabelMaxResolution, 2, 0);
    //mSpinBoxMaxResolution->setRange(1, 10000);
    //propertiesLayout->addWidget(mSpinBoxMaxResolution, 2, 1);

    propertiesLayout->addWidget(mLabelNumberViews, 2, 0);
    mSpinBoxNumberViews->setRange(1, 100);
    propertiesLayout->addWidget(mSpinBoxNumberViews, 2, 1);

    propertiesLayout->addWidget(mLabelNumberViewsFuse, 3, 0);
    mSpinBoxNumberViewsFuse->setRange(1, 100);
    propertiesLayout->addWidget(mSpinBoxNumberViewsFuse, 3, 1);

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
