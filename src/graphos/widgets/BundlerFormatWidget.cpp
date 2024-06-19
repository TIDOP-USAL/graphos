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

#include "BundlerFormatWidget.h"

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
TL_DEFAULT_WARNINGS

namespace graphos
{



BundlerFormatWidget::BundlerFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
    BundlerFormatWidget::initUI();
    BundlerFormatWidget::initSignalAndSlots();
}

void BundlerFormatWidget::initUI()
{
    this->setWindowTitle("Bundler");
    this->setObjectName("BundlerFormatWidgett");

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

void BundlerFormatWidget::initSignalAndSlots()
{
    connect(mLineEditFile, &QLineEdit::textChanged, this, &BundlerFormatWidget::fileChanged);
    connect(mPushButtonSelectPath, &QAbstractButton::clicked, this, &BundlerFormatWidget::onPushButtonSelectPath);
}

void BundlerFormatWidget::clear()
{
    const QSignalBlocker blocker(mLineEditFile);

    mLineEditFile->clear();
}

void BundlerFormatWidget::update()
{
}

void BundlerFormatWidget::retranslate()
{
    mLabelFile->setText(QApplication::translate("BundlerFormatWidgett", "Bundler File", nullptr));
}

void BundlerFormatWidget::setFile(const QString &file)
{
    const QSignalBlocker blocker(mLineEditFile);
    mLineEditFile->setText(file);
}

QString BundlerFormatWidget::file() const
{
    return mLineEditFile->text();
}

void BundlerFormatWidget::onPushButtonSelectPath()
{
    QString bundler_file = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Bundler file"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Bundler v3 (*.out)"));

    if (!bundler_file.isEmpty()) {
        mLineEditFile->setText(bundler_file);
    }

    update();
}


} // namespace graphos
