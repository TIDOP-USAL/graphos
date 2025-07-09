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

#include "ExportCameraPosesView.h"

#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>

namespace graphos
{

ExportCameraPosesViewImp::ExportCameraPosesViewImp(QWidget *parent)
  : ExportCameraPosesView(parent)
{
    ExportCameraPosesViewImp::initUI();
    ExportCameraPosesViewImp::initSignalAndSlots();
}

void ExportCameraPosesViewImp::initUI()
{
    this->setObjectName(QStringLiteral("ExportCameraPosesView"));
    this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));
    this->resize(400, 200);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelFormat = new QLabel(this);
    mLabelFormat->setMaximumSize(QSize(71, 16777215));
    grid_layout->addWidget(mLabelFormat, 0, 0, 1, 1);

    mComboBoxFormat = new QComboBox(this);
    grid_layout->addWidget(mComboBoxFormat, 0, 1, 1, 1);

    auto widget_feature_extractor = new QWidget();
    mGridLayoutFormat = new QGridLayout(widget_feature_extractor);
    mGridLayoutFormat->setContentsMargins(0, 0, 0, 0);
    grid_layout->addWidget(widget_feature_extractor, 1, 0, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 2, 0, 1, 2);

    this->retranslate();
    this->update();
}

void ExportCameraPosesViewImp::initSignalAndSlots()
{
    connect(mComboBoxFormat, &QComboBox::currentTextChanged, this, &ExportCameraPosesView::formatChange);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &ExportCameraPosesView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void ExportCameraPosesViewImp::clear()
{
    const QSignalBlocker blocker_combo_box_format(mComboBoxFormat);
    mComboBoxFormat->clear();
    update();
}

void ExportCameraPosesViewImp::update()
{
}

void ExportCameraPosesViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("ExportCameraPosesView", "Export Cameras", nullptr));
    mLabelFormat->setText(QApplication::translate("ExportCameraPosesView", "Format:", nullptr));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("ExportCameraPosesView", "Cancel", nullptr));
    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("ExportCameraPosesView", "Save", nullptr));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("ExportCameraPosesView", "Help", nullptr));
}

void ExportCameraPosesViewImp::addFormatWidget(QWidget *formatWidget)
{
    mComboBoxFormat->addItem(formatWidget->windowTitle());
    mGridLayoutFormat->addWidget(formatWidget, 0, 0, 1, 2);
    formatWidget->setVisible(false);

    this->update();
}

auto ExportCameraPosesViewImp::format() const -> QString
{
    return mComboBoxFormat->currentText();
}

void ExportCameraPosesViewImp::setCurrentFormat(const QString &format)
{
    const QSignalBlocker blocker_combo_box_format(mComboBoxFormat);
    mComboBoxFormat->setCurrentText(format);

    for (int idx = 0; idx < mGridLayoutFormat->count(); idx++) {
        QLayoutItem *item = mGridLayoutFormat->itemAt(idx);
        if (dynamic_cast<QWidgetItem *>(item)) {
            if (item->widget()->windowTitle().compare(format) == 0)
                item->widget()->setVisible(true);
            else
                item->widget()->setVisible(false);
        }
    }
}

} // namespace graphos
