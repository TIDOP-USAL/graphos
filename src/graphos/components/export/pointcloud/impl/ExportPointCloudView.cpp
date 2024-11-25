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
 
#include "ExportPointCloudView.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace graphos
{

ExportPointCloudViewImp::ExportPointCloudViewImp(QWidget *parent)
  : ExportPointCloudView(parent)
{
    ExportPointCloudViewImp::initUI();
    ExportPointCloudViewImp::initSignalAndSlots();
}

auto ExportPointCloudViewImp::format() -> QString
{
    for (int idx = 0; idx < mGridLayoutFormat->count(); idx++) {
        QLayoutItem *item = mGridLayoutFormat->itemAt(idx);
        if (dynamic_cast<QWidgetItem *>(item)) {
            if (item->widget()->isVisible())
                return item->widget()->windowTitle();
        }
    }

    return {};
}

void ExportPointCloudViewImp::addFormat(QWidget *format)
{
    mGridLayoutFormat->addWidget(format, 1, 0, 1, 2);
    format->setVisible(false);

    update();
}

void ExportPointCloudViewImp::setFormat(const QString &format)
{
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

auto ExportPointCloudViewImp::crs() const -> QString
{
    return mLineEditCRS->text();
}

void ExportPointCloudViewImp::setCrs(const QString &crs)
{
    mLineEditCRS->setText(crs);
}

void ExportPointCloudViewImp::initUI()
{
    this->setObjectName(QString("DemView"));
    this->resize(200, 300);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelCRS = new QLabel(this);
    grid_layout->addWidget(mLabelCRS, 0, 0, 1, 1);
    mLineEditCRS = new QLineEdit(this);
    mLineEditCRS->setDisabled(true);
    grid_layout->addWidget(mLineEditCRS, 0, 1, 1, 1);
    mPushButtonCRS = new QPushButton(this);
    mPushButtonCRS->setMaximumSize(QSize(31, 28));
    mPushButtonCRS->setText("...");
    grid_layout->addWidget(mPushButtonCRS, 0, 2, 1, 1);

    auto widget_format = new QWidget();
    mGridLayoutFormat = new QGridLayout(widget_format);
    mGridLayoutFormat->setContentsMargins(0, 0, 0, 0);
    grid_layout->addWidget(widget_format, 1, 0, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 2, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void ExportPointCloudViewImp::initSignalAndSlots()
{
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &ExportPointCloudViewImp::crs_change);
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &ExportPointCloudViewImp::update);
    connect(mPushButtonCRS, &QAbstractButton::clicked, this, &ExportPointCloudViewImp::select_crs);
    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &ExportPointCloudView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void ExportPointCloudViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("ExportPointCloudView", "Densification", nullptr));
    mLabelCRS->setText(QCoreApplication::translate("ExportPointCloudView", "CRS"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("ExportPointCloudView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Ok)->setText(QApplication::translate("ExportPointCloudView", "Ok"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("ExportPointCloudView", "Help"));
}

void ExportPointCloudViewImp::clear()
{
    const QSignalBlocker blocker_crs(mLineEditCRS);
    mLineEditCRS->clear();
}

void ExportPointCloudViewImp::update()
{
    mButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!mLineEditCRS->text().isEmpty());
}

//void ExportPointCloudViewImp::init()
//{
//	QFileDialog::setWindowTitle(tr("Export Point Cloud"));
//	QFileDialog::setNameFilter(tr("PLY (*.ply)"));
//	//QFileDialog::setNameFilter(tr("LAS (*.las)"));
//	//QFileDialog::setFileMode(QFileDialog::ExistingFile);
//	QFileDialog::setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
//}
 
} // namespace graphos