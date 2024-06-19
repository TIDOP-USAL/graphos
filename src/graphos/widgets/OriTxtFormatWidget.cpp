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

#include "OriTxtFormatWidget.h"

TL_DISABLE_WARNINGS
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QApplication>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QComboBox>
TL_DEFAULT_WARNINGS

namespace graphos
{



OriTxtFormatWidget::OriTxtFormatWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGroupBox(new QGroupBox(this)),
    mLabelRotation(new QLabel(this)),
    mComboBoxRotation(new QComboBox(this)),
    mLabelFile(new QLabel(this)),
    mLineEditFile(new QLineEdit(this)),
    mPushButtonSelectPath(new QPushButton(this))
{
    OriTxtFormatWidget::initUI();
    OriTxtFormatWidget::initSignalAndSlots();
}

void OriTxtFormatWidget::initUI()
{
    this->setWindowTitle("TXT");
    this->setObjectName("OriTxtFormatWidget");

    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);

    layout->addWidget(mGroupBox);

    QGridLayout *properties_layout = new QGridLayout();
    mGroupBox->setLayout(properties_layout);

    properties_layout->addWidget(mLabelRotation, 0, 0, 1, 1);
    mComboBoxRotation->addItem("Omega Phi Kappa");
    mComboBoxRotation->addItem("Quaternions");
    properties_layout->addWidget(mComboBoxRotation, 0, 1, 1, 1);

    properties_layout->addWidget(mLabelFile, 1, 0, 1, 1);
    properties_layout->addWidget(mLineEditFile, 1, 1, 1, 1);
    mPushButtonSelectPath->setMaximumSize(QSize(23, 16777215));
    mPushButtonSelectPath->setText("...");
    properties_layout->addWidget(mPushButtonSelectPath, 1, 2, 1, 1);

    OriTxtFormatWidget::retranslate();
    OriTxtFormatWidget::clear(); /// set default values
    OriTxtFormatWidget::update();
}

void OriTxtFormatWidget::initSignalAndSlots()
{
    connect(mComboBoxRotation, &QComboBox::currentTextChanged, this, &OriTxtFormatWidget::rotationChanged);
    connect(mLineEditFile, &QLineEdit::textChanged, this, &OriTxtFormatWidget::fileChanged);
    connect(mPushButtonSelectPath, &QAbstractButton::clicked, this, &OriTxtFormatWidget::onPushButtonSelectPath);
}

void OriTxtFormatWidget::clear()
{
    const QSignalBlocker blocker(mLineEditFile);
    const QSignalBlocker blocker2(mComboBoxRotation);

    mComboBoxRotation->setCurrentText("Omega Phi Kappa");
    mLineEditFile->clear();
}

void OriTxtFormatWidget::update()
{
}

void OriTxtFormatWidget::retranslate()
{
    mLabelFile->setText(QApplication::translate("OriTxtFormatWidget", "File", nullptr));
}

void OriTxtFormatWidget::setFile(const QString &file)
{
    const QSignalBlocker blocker(mLineEditFile);
    mLineEditFile->setText(file);
}

QString OriTxtFormatWidget::file() const
{
    return mLineEditFile->text();
}

QString OriTxtFormatWidget::rotation() const
{
    return mComboBoxRotation->currentText();
}

void OriTxtFormatWidget::setRotation(const QString &rotation)
{
    const QSignalBlocker blocker(mComboBoxRotation);
    mComboBoxRotation->setCurrentText(rotation);
}

void OriTxtFormatWidget::onPushButtonSelectPath()
{
    QString file_path = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Txt file"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Orientation plain text (*.txt)"));

    if (!file_path.isEmpty()) {
        mLineEditFile->setText(file_path);
    }

    update();
}


} // namespace graphos
