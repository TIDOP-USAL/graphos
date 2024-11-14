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

#include "PlyFormatWidget.h"

#include <QRadioButton>

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{

PlyFormatWidget::PlyFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
    PlyFormatWidget::initUI();
    PlyFormatWidget::initSignalAndSlots();
}

auto PlyFormatWidget::format() const -> Format
{
    return mFormat.flags();
}

void PlyFormatWidget::setFormat(Format format)
{
    mFormat = format;
}

void PlyFormatWidget::enableExportColors(bool active)
{
    mCheckBoxColors->setChecked(active);
}

auto PlyFormatWidget::isExportColorsEnabled() const -> bool
{
    return mCheckBoxColors->isChecked();
}

void PlyFormatWidget::enableExportNormals(bool active)
{
    mCheckBoxNormals->setChecked(active);
}

auto PlyFormatWidget::isExportNormalsEnabled() const -> bool
{
    return mCheckBoxNormals->isChecked();
}

void PlyFormatWidget::initUI()
{
    this->setWindowTitle("Point Cloud PLY Format");
    this->setObjectName("PlyFormatWidget");

    auto layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    mGroupBoxFormat = new QGroupBox(this);

    auto grid_layout_format = new QGridLayout(mGroupBoxFormat);
    mRadioButtonBinary = new QRadioButton(mGroupBoxFormat);
    grid_layout_format->addWidget(mRadioButtonBinary, 0, 0, 1, 1);
    mRadioButtonText = new QRadioButton(mGroupBoxFormat);
    grid_layout_format->addWidget(mRadioButtonText, 0, 1, 1, 1);

    layout->addWidget(mGroupBoxFormat);

    mCheckBoxColors = new QCheckBox(this);
    layout->addWidget(mCheckBoxColors, 1, 0, 1, 1);

    mCheckBoxNormals = new QCheckBox(this);
    layout->addWidget(mCheckBoxNormals, 2, 0, 1, 1);

    retranslate();
    clear(); /// set default values
    update();
}

void PlyFormatWidget::initSignalAndSlots()
{
}

void PlyFormatWidget::clear()
{
    const QSignalBlocker blocker(mRadioButtonBinary);
    mRadioButtonBinary->setChecked(true);

    const QSignalBlocker blocker_colors(mCheckBoxColors);
    mCheckBoxColors->setChecked(true);

    const QSignalBlocker blocker_normals(mCheckBoxNormals);
    mCheckBoxNormals->setChecked(true);
}

void PlyFormatWidget::update()
{
}

void PlyFormatWidget::retranslate()
{
    mGroupBoxFormat->setTitle(QCoreApplication::translate("PlyFormatWidget", "Format", nullptr));
    mRadioButtonBinary->setText(QCoreApplication::translate("PlyFormatWidget", "Binary", nullptr));
    mRadioButtonText->setText(QCoreApplication::translate("PlyFormatWidget", "Text", nullptr));
    mCheckBoxColors->setText(QApplication::translate("PlyFormatWidget", "Export colors"));
    mCheckBoxNormals->setText(QApplication::translate("PlyFormatWidget", "Export normals"));
}

} // namespace graphos


