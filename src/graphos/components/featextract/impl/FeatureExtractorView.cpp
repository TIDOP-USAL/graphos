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

#include "FeatureExtractorView.h"

#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QApplication>

namespace graphos
{

FeatureExtractorViewImp::FeatureExtractorViewImp(QWidget *parent)
  : FeatureExtractorView(parent),
    mButtonBox(new QDialogButtonBox(this))
{
    this->initUI();
    this->initSignalAndSlots();
}

FeatureExtractorViewImp::~FeatureExtractorViewImp()
{
}

void FeatureExtractorViewImp::initUI()
{
    this->setObjectName(QString("FeatureExtractorView"));
    this->resize(380, 300);

    QGridLayout* gridLayout = new QGridLayout();
    this->setLayout(gridLayout);

    mCheckBoxFullImage = new QCheckBox(this);
    gridLayout->addWidget(mCheckBoxFullImage, 0, 0, 1, 1);

    mLabelMaxImageSize = new QLabel(this);
    gridLayout->addWidget(mLabelMaxImageSize, 1, 0, 1, 1);
    mSpinBoxMaxImageSize = new QSpinBox(this);
    mSpinBoxMaxImageSize->setRange(100, 100000);
    gridLayout->addWidget(mSpinBoxMaxImageSize, 1, 1, 1, 1);

    mLabelDetectorDescriptor = new QLabel(this);
    gridLayout->addWidget(mLabelDetectorDescriptor, 2, 0, 1, 1);
    mComboBoxDetectorDescriptor = new QComboBox(this);
    gridLayout->addWidget(mComboBoxDetectorDescriptor, 2, 1, 1, 1);

    QWidget* widgetFeatureExtractor = new QWidget(this);
    mGridLayoutDetectorDescriptor = new QGridLayout(widgetFeatureExtractor);
    mGridLayoutDetectorDescriptor->setContentsMargins(0, 0, 0, 0);
    gridLayout->addWidget(widgetFeatureExtractor, 3, 0, 1, 2);

    gridLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding), 4, 0, 1, 2);

    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    gridLayout->addWidget(mButtonBox, 4, 0, 1, 2);

    this->retranslate();
    this->clear();
    this->update();
}

void FeatureExtractorViewImp::initSignalAndSlots()
{
    connect(mCheckBoxFullImage, &QAbstractButton::clicked, this, &FeatureExtractorViewImp::onCheckBoxFullImageChange);
    connect(mComboBoxDetectorDescriptor, &QComboBox::currentTextChanged, this, &FeatureExtractorView::detectorDescriptorChange);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &FeatureExtractorView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void FeatureExtractorViewImp::clear()
{
    const QSignalBlocker blockerMaxImageSize(mSpinBoxMaxImageSize);

    mSpinBoxMaxImageSize->setValue(3000);
    mCheckBoxFullImage->setChecked(false);
}

void FeatureExtractorViewImp::update()
{
    int detectorDescriptorCount = mComboBoxDetectorDescriptor->count();
    mLabelDetectorDescriptor->setVisible(detectorDescriptorCount > 1);
    mComboBoxDetectorDescriptor->setVisible(detectorDescriptorCount > 1);
}

void FeatureExtractorViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("FeatureExtractorView", "Feature Extractor"));
    mLabelMaxImageSize->setText(QApplication::translate("FeatureExtractorView", "Max Image Size:"));
    mCheckBoxFullImage->setText(QApplication::translate("FeatureExtractorView", "Full Image Size"));
    mLabelDetectorDescriptor->setText(QApplication::translate("FeatureExtractorView", "Keypoint Detector/Descriptor:"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("FeatureExtractorView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("FeatureExtractorView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("FeatureExtractorView", "Help"));
}

void FeatureExtractorViewImp::onCheckBoxFullImageChange()
{
    mSpinBoxMaxImageSize->setEnabled(!mCheckBoxFullImage->isChecked());
}

void FeatureExtractorViewImp::addDetectorDescriptor(QWidget* detectorExtractor)
{
    mComboBoxDetectorDescriptor->addItem(detectorExtractor->windowTitle());
    mGridLayoutDetectorDescriptor->addWidget(detectorExtractor, 0, 0, 1, 2);
    detectorExtractor->setVisible(false);

    this->update();
}

QString FeatureExtractorViewImp::currentDetectorDescriptor() const
{
    return mComboBoxDetectorDescriptor->currentText();
}

int FeatureExtractorViewImp::maxImageSize() const
{
    return mSpinBoxMaxImageSize->value();
}

bool FeatureExtractorViewImp::fullImageSize() const
{
    return mCheckBoxFullImage->isChecked();
}

void FeatureExtractorViewImp::setCurrentDetectorDescriptor(const QString& detectorExtractor)
{
    const QSignalBlocker blocker(mComboBoxDetectorDescriptor);
    mComboBoxDetectorDescriptor->setCurrentText(detectorExtractor);

    for (int idx = 0; idx < mGridLayoutDetectorDescriptor->count(); idx++) {
        QLayoutItem* const item = mGridLayoutDetectorDescriptor->itemAt(idx);
        if (dynamic_cast<QWidgetItem*>(item)) {
            if (item->widget()->windowTitle().compare(detectorExtractor) == 0)
                item->widget()->setVisible(true);
            else
                item->widget()->setVisible(false);
        }
    }
}

void FeatureExtractorViewImp::setMaxImageSize(int imageSize)
{
    const QSignalBlocker blockerMaxImageSize(mSpinBoxMaxImageSize);
    mSpinBoxMaxImageSize->setValue(imageSize);
}

void FeatureExtractorViewImp::setFullImageSize(bool fullImageSize)
{
    mCheckBoxFullImage->setChecked(fullImageSize);
}

} // namespace graphos
