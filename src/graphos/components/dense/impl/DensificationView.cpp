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

#include "DensificationView.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QApplication>
#include <QCheckBox>

namespace graphos
{

DensificationViewImp::DensificationViewImp(QWidget *parent)
  : DensificationView(parent)
{
    DensificationViewImp::initUI();
    DensificationViewImp::initSignalAndSlots();
}

void DensificationViewImp::initUI()
{
    this->setObjectName(QString("DensificationView"));
    this->resize(443, 248);

    mGridLayout = new QGridLayout(this);

    mLabelDensification = new QLabel(this);
    mGridLayout->addWidget(mLabelDensification, 0, 0, 1, 1);

    mComboBoxDensification = new QComboBox(this);
    mGridLayout->addWidget(mComboBoxDensification, 0, 1, 1, 1);

    QWidget *widget_densification = new QWidget();
    mGridLayoutDensification = new QGridLayout(widget_densification);
    mGridLayoutDensification->setContentsMargins(0, 0, 0, 0);
    mGridLayout->addWidget(widget_densification, 1, 0, 1, 2);

    mCheckBoxAutoSegmentation = new QCheckBox(this);
    mGridLayout->addWidget(mCheckBoxAutoSegmentation, 2, 0, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    mGridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void DensificationViewImp::initSignalAndSlots()
{
    connect(mComboBoxDensification, &QComboBox::currentTextChanged, this, &DensificationView::densificationChanged);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &DensificationView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void DensificationViewImp::clear()
{
    mCheckBoxAutoSegmentation->setChecked(false);
}

void DensificationViewImp::update()
{
    mComboBoxDensification->setVisible(mComboBoxDensification->count() > 1);
    mLabelDensification->setVisible(mComboBoxDensification->count() > 1);
}

void DensificationViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("DensificationView", "Densification", nullptr));
    mLabelDensification->setText(QApplication::translate("DensificationView", "Method:", nullptr));
    mCheckBoxAutoSegmentation->setText(QApplication::translate("DensificationView", "Point cloud automatic segmentation", nullptr));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("FeatureExtractorView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("FeatureExtractorView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("FeatureExtractorView", "Help"));
}

QString DensificationViewImp::currentDensificationMethod() const
{
    return mComboBoxDensification->currentText();
}

bool DensificationViewImp::autoSegmentation() const
{
    return mCheckBoxAutoSegmentation->isChecked();
}

void DensificationViewImp::addDensification(QWidget *densification)
{
    mComboBoxDensification->addItem(densification->windowTitle());
    mGridLayoutDensification->addWidget(densification, 1, 0, 1, 2);
    densification->setVisible(false);

    update();
}

void DensificationViewImp::setCurrentDensificationMethod(const QString &densificationMethod)
{
    const QSignalBlocker blocker(mComboBoxDensification);
    mComboBoxDensification->setCurrentText(densificationMethod);

    for (int idx = 0; idx < mGridLayoutDensification->count(); idx++) {
        QLayoutItem *item = mGridLayoutDensification->itemAt(idx);
        if (dynamic_cast<QWidgetItem *>(item)) {
            if (item->widget()->windowTitle().compare(densificationMethod) == 0)
                item->widget()->setVisible(true);
            else
                item->widget()->setVisible(false);
        }
    }
}

void DensificationViewImp::setAutoSegmentation(bool autoSegmentation)
{
    mCheckBoxAutoSegmentation->setChecked(autoSegmentation);
}

} // End namespace graphos
