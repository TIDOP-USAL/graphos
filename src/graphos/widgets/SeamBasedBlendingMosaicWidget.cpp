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

#include "SeamBasedBlendingMosaicWidget.h"

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QApplication>
#include <QComboBox>
TL_DEFAULT_WARNINGS

namespace graphos
{

SeamBasedBlendingMosaicWidget::SeamBasedBlendingMosaicWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mLabelCompensator(new QLabel(this)),
    mExposureCompensator(new QComboBox(this)),
    mLabelSeamFinder(new QLabel(this)),
    mSeamFinder(new QComboBox(this)),
    mLabelBlenderType(new QLabel(this)),
    mBlenderType(new QComboBox(this))
{
    SeamBasedBlendingMosaicWidget::initUI();
    SeamBasedBlendingMosaicWidget::initSignalAndSlots();
}

SeamBasedBlendingMosaicWidget::~SeamBasedBlendingMosaicWidget()
{

}

void SeamBasedBlendingMosaicWidget::initUI()
{
    this->setObjectName("SBB");
    this->setWindowTitle("Seam-based Blending");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mLabelCompensator, 0, 0);
    mExposureCompensator->addItem("No");
    mExposureCompensator->addItem("Gain");
    mExposureCompensator->addItem("Gain Blocks");
    mExposureCompensator->addItem("Channels");
    mExposureCompensator->addItem("Channels Blocks");
    mExposureCompensator->setItemData(0, "NO");
    mExposureCompensator->setItemData(1, "GAIN");
    mExposureCompensator->setItemData(2, "GAIN_BLOCKS");
    mExposureCompensator->setItemData(3, "CHANNELS");
    mExposureCompensator->setItemData(4, "CHANNELS_BLOCKS");
    layout->addWidget(mExposureCompensator, 0, 1);

    layout->addWidget(mLabelSeamFinder, 1, 0);
    mSeamFinder->addItem("Graph Cut");
    mSeamFinder->addItem("VoronoiSeamFinder");
    mSeamFinder->setItemData(0, "GRAPH_CUT");
    mSeamFinder->setItemData(1, "VORONOI");
    layout->addWidget(mSeamFinder, 1, 1);

    layout->addWidget(mLabelBlenderType, 2, 0);
    mBlenderType->addItem("No");
    mBlenderType->addItem("Feather");
    mBlenderType->addItem("Multi-Band");
    mBlenderType->setItemData(0, "NO");
    mBlenderType->setItemData(1, "FEATHER");
    mBlenderType->setItemData(2, "MULTI_BAND");
    layout->addWidget(mBlenderType, 2, 1);

    SeamBasedBlendingMosaicWidget::retranslate();
    SeamBasedBlendingMosaicWidget::clear();
    SeamBasedBlendingMosaicWidget::update();
}

void SeamBasedBlendingMosaicWidget::initSignalAndSlots()
{
    connect(mExposureCompensator, &QComboBox::currentTextChanged, this, &SeamBasedBlendingMosaicWidget::compensatorChange);
	connect(mSeamFinder, &QComboBox::currentTextChanged, this, &SeamBasedBlendingMosaicWidget::seamFinderChange);
	connect(mBlenderType, &QComboBox::currentTextChanged, this, &SeamBasedBlendingMosaicWidget::blenderTypeChange);
}

void SeamBasedBlendingMosaicWidget::clear()
{
    setExposureCompensator("GAIN");
    setSeamFinder("GRAPH_CUT");
    setBlenderType("FEATHER");
}

void SeamBasedBlendingMosaicWidget::update()
{

}

void SeamBasedBlendingMosaicWidget::retranslate()
{
	mLabelCompensator->setText(QApplication::translate("SeamBasedBlendingMosaicWidget", "Exposure Compensator", nullptr));
	mLabelSeamFinder->setText(QApplication::translate("SeamBasedBlendingMosaicWidget", "Seam Finder", nullptr));
	mLabelBlenderType->setText(QApplication::translate("SeamBasedBlendingMosaicWidget", "Blender Type", nullptr));
}

auto SeamBasedBlendingMosaicWidget::exposureCompensator() const -> QString
{
    return mExposureCompensator->currentData().toString();
}

auto SeamBasedBlendingMosaicWidget::seamFinder() const -> QString
{
    return mSeamFinder->currentData().toString();
}

auto SeamBasedBlendingMosaicWidget::blenderType() const -> QString
{
    return mBlenderType->currentData().toString();
}

void SeamBasedBlendingMosaicWidget::setExposureCompensator(const QString &exposureCompensator)
{
    const QSignalBlocker blocker(mExposureCompensator);
    mExposureCompensator->setCurrentIndex(mExposureCompensator->findData(exposureCompensator));
}

void SeamBasedBlendingMosaicWidget::setSeamFinder(const QString &seamFinder)
{
    const QSignalBlocker blocker(mSeamFinder);
    mSeamFinder->setCurrentIndex(mSeamFinder->findData(seamFinder));
}

void SeamBasedBlendingMosaicWidget::setBlenderType(const QString &blenderType)
{
	const QSignalBlocker blocker(mBlenderType);
	mBlenderType->setCurrentIndex(mBlenderType->findData(blenderType));
}

} // namespace graphos
