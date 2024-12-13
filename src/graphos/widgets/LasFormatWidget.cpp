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

#include "LasFormatWidget.h"

#include <QRadioButton>

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{

LasFormatWidget::LasFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
    LasFormatWidget::initUI();
    LasFormatWidget::initSignalAndSlots();
}

void LasFormatWidget::enableExportColors(bool active)
{
    mCheckBoxColors->setChecked(active);
}

auto LasFormatWidget::isExportColorsEnabled() const -> bool
{
    return mCheckBoxColors->isChecked();
}

void LasFormatWidget::enableExportNormals(bool active)
{
    mCheckBoxNormals->setChecked(active);
}

auto LasFormatWidget::isExportNormalsEnabled() const -> bool
{
    return mCheckBoxNormals->isChecked();
}

void LasFormatWidget::initUI()
{
    this->setWindowTitle("Point Cloud LAS Format");
    this->setObjectName("LasFormatWidget");

    auto layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    mCheckBoxColors = new QCheckBox(this);
    layout->addWidget(mCheckBoxColors, 0, 0, 1, 1);

    mCheckBoxNormals = new QCheckBox(this);
    layout->addWidget(mCheckBoxNormals, 1, 0, 1, 1);

    retranslate();
    clear(); /// set default values
    update();
}

void LasFormatWidget::initSignalAndSlots()
{
}

void LasFormatWidget::clear()
{
    const QSignalBlocker blocker_colors(mCheckBoxColors);
    mCheckBoxColors->setChecked(true);

    const QSignalBlocker blocker_normals(mCheckBoxNormals);
    mCheckBoxNormals->setChecked(true);
}

void LasFormatWidget::update()
{
}

void LasFormatWidget::retranslate()
{
    mCheckBoxColors->setText(QApplication::translate("LasFormatWidget", "Export colors"));
    mCheckBoxNormals->setText(QApplication::translate("LasFormatWidget", "Export normals"));
}

} // namespace graphos


