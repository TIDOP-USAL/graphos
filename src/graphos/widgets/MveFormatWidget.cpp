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

#include "MveFormatWidget.h"

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
TL_DEFAULT_WARNINGS

namespace graphos
{



MveFormatWidget::MveFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
    MveFormatWidget::initUI();
    MveFormatWidget::initSignalAndSlots();
}

void MveFormatWidget::initUI()
{
    this->setWindowTitle("MVE");
    this->setObjectName("MveOrientationFormatWidget");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *properties_layout = new QGridLayout();
    mGroupBox->setLayout(properties_layout);

    properties_layout->addWidget(mLabelFile, 0, 0, 1, 1);
    properties_layout->addWidget(mLineEditFile, 0, 1, 1, 1);
    mPushButtonSelectPath->setMaximumSize(QSize(23, 16777215));
    mPushButtonSelectPath->setText("...");
    properties_layout->addWidget(mPushButtonSelectPath, 0, 2, 1, 1);

    retranslate();
    clear(); /// set default values
    update();
}

void MveFormatWidget::initSignalAndSlots()
{
    connect(mLineEditFile, &QLineEdit::textChanged, this, &MveFormatWidget::fileChanged);
    connect(mPushButtonSelectPath, &QAbstractButton::clicked, this, &MveFormatWidget::onPushButtonSelectPath);
}

void MveFormatWidget::clear()
{
    const QSignalBlocker blocker(mLineEditFile);

    mLineEditFile->clear();
}

void MveFormatWidget::update()
{
}

void MveFormatWidget::retranslate()
{
    mLabelFile->setText(QApplication::translate("MveOrientationFormatWidget", "MVE export path", nullptr));
}

void MveFormatWidget::setFile(const QString &file)
{
    const QSignalBlocker blocker(mLineEditFile);
    mLineEditFile->setText(file);
}

QString MveFormatWidget::file() const
{
    return mLineEditFile->text();
}

void MveFormatWidget::onPushButtonSelectPath()
{
    QString export_path = QFileDialog::getExistingDirectory(this,
                                                         tr("Export directory"),
                                                         "",
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!export_path.isEmpty()) {
        mLineEditFile->setText(export_path);
    }

    update();
}


} // namespace graphos
