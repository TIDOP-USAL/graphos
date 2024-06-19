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

#include "PointCloudCSVFormatWidget.h"

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
TL_DEFAULT_WARNINGS

namespace graphos
{

PointCloudCSVFormatWidget::PointCloudCSVFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent)
{
    PointCloudCSVFormatWidget::initUI();
    PointCloudCSVFormatWidget::initSignalAndSlots();
}

void PointCloudCSVFormatWidget::initUI()
{
    this->setWindowTitle("Point Cloud CSV Format");
    this->setObjectName("PointCloudCSVFormatWidget");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    mGroupBoxDelimiter = new QGroupBox(this);

    QGridLayout *grid_layout_delimiter = new QGridLayout(mGroupBoxDelimiter);
    mRadioButtonTab = new QRadioButton(mGroupBoxDelimiter);
    grid_layout_delimiter->addWidget(mRadioButtonTab, 0, 0, 1, 1);
    mRadioButtonComma = new QRadioButton(mGroupBoxDelimiter);
    grid_layout_delimiter->addWidget(mRadioButtonComma, 0, 1, 1, 1);
    mRadioButtonSpace = new QRadioButton(mGroupBoxDelimiter);
    grid_layout_delimiter->addWidget(mRadioButtonSpace, 0, 2, 1, 1);
    mRadioButtonSemicolon = new QRadioButton(mGroupBoxDelimiter);
    grid_layout_delimiter->addWidget(mRadioButtonSemicolon, 0, 3, 1, 1);
    layout->addWidget(mGroupBoxDelimiter);

    retranslate();
    clear(); /// set default values
    update();
}

void PointCloudCSVFormatWidget::initSignalAndSlots()
{
    connect(mRadioButtonTab, &QRadioButton::released, this, &PointCloudCSVFormatWidget::onDelimiterChanged);
    connect(mRadioButtonComma, &QRadioButton::released, this, &PointCloudCSVFormatWidget::onDelimiterChanged);
    connect(mRadioButtonSpace, &QRadioButton::released, this, &PointCloudCSVFormatWidget::onDelimiterChanged);
    connect(mRadioButtonSemicolon, &QRadioButton::released, this, &PointCloudCSVFormatWidget::onDelimiterChanged);
}

void PointCloudCSVFormatWidget::clear()
{
    const QSignalBlocker blocker(mRadioButtonSemicolon);

    mRadioButtonSemicolon->setChecked(true);
}

void PointCloudCSVFormatWidget::update()
{
}

void PointCloudCSVFormatWidget::retranslate()
{
    mGroupBoxDelimiter->setTitle(QCoreApplication::translate("PointCloudCSVFormatWidget", "Delimiter", nullptr));
    mRadioButtonTab->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Tab", nullptr));
    mRadioButtonComma->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Comma", nullptr));
    mRadioButtonSpace->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Space", nullptr));
    mRadioButtonSemicolon->setText(QCoreApplication::translate("PointCloudCSVFormatWidget", "Semicolon", nullptr));
}

QString PointCloudCSVFormatWidget::delimiter() const
{
    QString delimiter;
    if (mRadioButtonTab->isChecked()) {
        delimiter = "\t";
    } else if (mRadioButtonComma->isChecked()) {
        delimiter = ",";
    } else if (mRadioButtonSpace->isChecked()) {
        delimiter = " ";
    } else if (mRadioButtonSemicolon->isChecked()) {
        delimiter = ";";
    }
    return delimiter;
}

void PointCloudCSVFormatWidget::setDelimiter(const QString &delimiter)
{
    if (delimiter.compare("\t") == 0) {
        mRadioButtonTab->setChecked(true);
    } else if (delimiter.compare(",") == 0) {
        mRadioButtonComma->setChecked(true);
    } else if (delimiter.compare(" ") == 0) {
        mRadioButtonSpace->setChecked(true);
    } else if (delimiter.compare(";") == 0) {
        mRadioButtonSemicolon->setChecked(true);
    }
}

void PointCloudCSVFormatWidget::onDelimiterChanged()
{
    emit delimiterChanged(this->delimiter());
}

} // namespace graphos


