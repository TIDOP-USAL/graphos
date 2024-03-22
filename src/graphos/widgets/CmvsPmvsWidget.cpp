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

#include "CmvsPmvsWidget.h"

TL_DISABLE_WARNINGS
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{

CmvsPmvsWidget::CmvsPmvsWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGroupBox(new QGroupBox(this)),
    mCheckBoxUseVisibilityInformation(new QCheckBox(this)),
    mLabelImagesPerCluster(new QLabel(this)),
    mSpinBoxImagesPerCluster(new QSpinBox(this)),
    mLabelLevel(new QLabel(this)),
    mSpinBoxLevel(new QSpinBox(this)),
    mLabelCellSize(new QLabel(this)),
    mSpinBoxCellSize(new QSpinBox(this)),
    mLabelThreshold(new QLabel(this)),
    mDoubleSpinBoxThreshold(new QDoubleSpinBox(this)),
    mLabelWindowSize(new QLabel(this)),
    mSpinBoxWindowSize(new QSpinBox(this)),
    mLabelMinimunImageNumber(new QLabel(this)),
    mSpinBoxMinimunImageNumber(new QSpinBox(this))
{
    CmvsPmvsWidget::initUI();
    CmvsPmvsWidget::initSignalAndSlots();
}

CmvsPmvsWidget::~CmvsPmvsWidget()
{

}

bool CmvsPmvsWidget::useVisibilityInformation() const
{
    return mCheckBoxUseVisibilityInformation->isChecked();
}

int CmvsPmvsWidget::imagesPerCluster() const
{
    return mSpinBoxImagesPerCluster->value();
}

int CmvsPmvsWidget::level() const
{
    return mSpinBoxLevel->value();
}

int CmvsPmvsWidget::cellSize() const
{
    return mSpinBoxCellSize->value();
}

double CmvsPmvsWidget::threshold() const
{
    return mDoubleSpinBoxThreshold->value();
}

int CmvsPmvsWidget::windowSize() const
{
    return mSpinBoxWindowSize->value();
}

int CmvsPmvsWidget::minimunImageNumber() const
{
    return mSpinBoxMinimunImageNumber->value();
}

void CmvsPmvsWidget::setUseVisibilityInformation(bool useVisibilityInformation)
{
    mCheckBoxUseVisibilityInformation->setChecked(useVisibilityInformation);
}

void CmvsPmvsWidget::setImagesPerCluster(int imagesPerCluster)
{
    const QSignalBlocker blockerImagesPerCluster(mSpinBoxImagesPerCluster);
    mSpinBoxImagesPerCluster->setValue(imagesPerCluster);
}

void CmvsPmvsWidget::setLevel(int level)
{
    const QSignalBlocker blockerLevel(mSpinBoxLevel);
    mSpinBoxLevel->setValue(level);
}

void CmvsPmvsWidget::setCellSize(int cellSize)
{
    const QSignalBlocker blockerCellSize(mSpinBoxCellSize);
    mSpinBoxCellSize->setValue(cellSize);
}

void CmvsPmvsWidget::setThreshold(double threshold)
{
    const QSignalBlocker blockerThreshold(mDoubleSpinBoxThreshold);
    mDoubleSpinBoxThreshold->setValue(threshold);
}

void CmvsPmvsWidget::setWindowSize(int windowSize)
{
    const QSignalBlocker blockerWindowSize(mSpinBoxWindowSize);
    mSpinBoxWindowSize->setValue(windowSize);
}

void CmvsPmvsWidget::setMinimunImageNumber(int minimunImageNumber)
{
    const QSignalBlocker blockerMinimunImageNumber(mSpinBoxMinimunImageNumber);
    mSpinBoxMinimunImageNumber->setValue(minimunImageNumber);
}

void CmvsPmvsWidget::update()
{

}

void CmvsPmvsWidget::retranslate()
{
    mGroupBox->setTitle(QApplication::translate("CmvsPmvsWidget", "CMVS/PMVS", nullptr));
    mCheckBoxUseVisibilityInformation->setText(QApplication::translate("CmvsPmvsWidget", "Use Visibility Information:"));
    mLabelImagesPerCluster->setText(QApplication::translate("CmvsPmvsWidget", "Images Per Cluster:"));
    mLabelLevel->setText(QApplication::translate("CmvsPmvsWidget", "Level:"));
    mLabelCellSize->setText(QApplication::translate("CmvsPmvsWidget", "Cell Size:"));
    mLabelThreshold->setText(QApplication::translate("CmvsPmvsWidget", "Threshold:"));
    mLabelWindowSize->setText(QApplication::translate("CmvsPmvsWidget", "Window Size:"));
    mLabelMinimunImageNumber->setText(QApplication::translate("CmvsPmvsWidget", "Minimun Image Number:"));
}

void CmvsPmvsWidget::clear()
{
    const QSignalBlocker blockerImagesPerCluster(mSpinBoxImagesPerCluster);
    const QSignalBlocker blockerLevel(mSpinBoxLevel);
    const QSignalBlocker blockerCellSize(mSpinBoxCellSize);
    const QSignalBlocker blockerThreshold(mDoubleSpinBoxThreshold);
    const QSignalBlocker blockerWindowSize(mSpinBoxWindowSize);
    const QSignalBlocker blockerMinimunImageNumber(mSpinBoxMinimunImageNumber);

    mCheckBoxUseVisibilityInformation->setChecked(true);
    mSpinBoxImagesPerCluster->setValue(100);
    mSpinBoxLevel->setValue(1);
    mSpinBoxCellSize->setValue(2);
    mDoubleSpinBoxThreshold->setValue(0.7);
    mSpinBoxWindowSize->setValue(7);
    mSpinBoxMinimunImageNumber->setValue(3);
}

void CmvsPmvsWidget::initUI()
{
    this->setWindowTitle("CMVS/PMVS");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *propertiesLayout = new QGridLayout();
    mGroupBox->setLayout(propertiesLayout);

    propertiesLayout->addWidget(mCheckBoxUseVisibilityInformation, 0, 0, 1, 2);

    propertiesLayout->addWidget(mLabelImagesPerCluster, 1, 0);
    mSpinBoxImagesPerCluster->setRange(1, 1000);
    propertiesLayout->addWidget(mSpinBoxImagesPerCluster, 1, 1);

    propertiesLayout->addWidget(mLabelLevel, 2, 0);
    mSpinBoxLevel->setRange(0, 6);
    propertiesLayout->addWidget(mSpinBoxLevel, 2, 1);

    propertiesLayout->addWidget(mLabelCellSize, 3, 0);
    mSpinBoxCellSize->setRange(1, 7);
    propertiesLayout->addWidget(mSpinBoxCellSize, 3, 1);

    propertiesLayout->addWidget(mLabelThreshold, 4, 0);
    mDoubleSpinBoxThreshold->setRange(0.0, 1.0);
    mDoubleSpinBoxThreshold->setDecimals(2);
    mDoubleSpinBoxThreshold->setSingleStep(0.1);
    propertiesLayout->addWidget(mDoubleSpinBoxThreshold, 4, 1);

    propertiesLayout->addWidget(mLabelWindowSize, 5, 0);
    mSpinBoxWindowSize->setRange(5, 20);
    propertiesLayout->addWidget(mSpinBoxWindowSize, 5, 1);

    propertiesLayout->addWidget(mLabelMinimunImageNumber, 6, 0);
    mSpinBoxMinimunImageNumber->setRange(2, 6);
    propertiesLayout->addWidget(mSpinBoxMinimunImageNumber, 6, 1);

    retranslate();
    clear(); /// set default values
    update();
}

void CmvsPmvsWidget::initSignalAndSlots()
{
    connect(mCheckBoxUseVisibilityInformation, &QAbstractButton::clicked, this, &CmvsPmvsWidget::useVisibilityInformationChanged);
    connect(mSpinBoxImagesPerCluster, QOverload<int>::of(&QSpinBox::valueChanged), this, &CmvsPmvsWidget::imagesPerClusterChanged);
    connect(mSpinBoxLevel, QOverload<int>::of(&QSpinBox::valueChanged), this, &CmvsPmvsWidget::levelChanged);
    connect(mSpinBoxCellSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &CmvsPmvsWidget::cellSizeChanged);
    connect(mDoubleSpinBoxThreshold, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CmvsPmvsWidget::thresholdChanged);
    connect(mSpinBoxWindowSize, QOverload<int>::of(&QSpinBox::valueChanged), this, &CmvsPmvsWidget::windowSizeChanged);
    connect(mSpinBoxMinimunImageNumber, QOverload<int>::of(&QSpinBox::valueChanged), this, &CmvsPmvsWidget::minimunImageNumberChanged);
}


} // namespace graphos
