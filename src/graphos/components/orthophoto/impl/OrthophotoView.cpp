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

#include "OrthophotoView.h"

#include <QApplication>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>

namespace graphos
{

OrthophotoViewImp::OrthophotoViewImp(QWidget *parent)
  : OrthophotoView(parent)
{
    OrthophotoViewImp::initUI();
    OrthophotoViewImp::initSignalAndSlots();
}

OrthophotoViewImp::~OrthophotoViewImp()
{
}

void OrthophotoViewImp::initUI()
{
    this->setObjectName(QString("OrthophotoView"));
    //this->resize(380, 300);

    auto grid_layout = new QGridLayout();
    this->setLayout(grid_layout);

    mLabelCRS = new QLabel(this);
    grid_layout->addWidget(mLabelCRS, 0, 0, 1, 1);
    mLineEditCRS = new QLineEdit(this);
    mLineEditCRS->setMaximumWidth(200);
    mLineEditCRS->setDisabled(true);
    grid_layout->addWidget(mLineEditCRS, 0, 1, 1, 1);
    mPushButtonCRS = new QPushButton(this);
    mPushButtonCRS->setMaximumSize(QSize(31, 28));
    mPushButtonCRS->setText("...");
    grid_layout->addWidget(mPushButtonCRS, 0, 2, 1, 1);

    mGroupOrthoimages = new QGroupBox(this);
    auto orthoimages_layout = new QGridLayout(mGroupOrthoimages);

    mLabelGSD = new QLabel(this);
    orthoimages_layout->addWidget(mLabelGSD, 0, 0, 1, 1);
    mDoubleSpinBoxGSD = new QDoubleSpinBox(this);
    mDoubleSpinBoxGSD->setSingleStep(0.1);
    orthoimages_layout->addWidget(mDoubleSpinBoxGSD, 0, 1, 1, 2);

    mLabelmaxTiltAngle = new QLabel(this);
    orthoimages_layout->addWidget(mLabelmaxTiltAngle, 1, 0, 1, 1);
    mDoubleSpinBoxMaxTiltAngle = new QDoubleSpinBox(this);
    mDoubleSpinBoxMaxTiltAngle->setSingleStep(0.1);
    mDoubleSpinBoxMaxTiltAngle->setMinimum(5.);
    mDoubleSpinBoxMaxTiltAngle->setMaximum(60.);
    orthoimages_layout->addWidget(mDoubleSpinBoxMaxTiltAngle, 1, 1, 1, 2);

    mLabelInterpolation = new QLabel(this);
    orthoimages_layout->addWidget(mLabelInterpolation, 2, 0, 1, 1);
    mComboBoxInterpolation = new QComboBox(this);
    mComboBoxInterpolation->addItem("");
    mComboBoxInterpolation->addItem("");
    mComboBoxInterpolation->addItem("");
    mComboBoxInterpolation->setItemData(0, "NEAREST");
    mComboBoxInterpolation->setItemData(1, "BILINEAR");
    mComboBoxInterpolation->setItemData(2, "BICUBIC");
    orthoimages_layout->addWidget(mComboBoxInterpolation, 2, 1, 1, 2);

    grid_layout->addWidget(mGroupOrthoimages, 1, 0, 1, 3);

    mGroupOrthomosaic = new QGroupBox(this);
    auto orthomosaic_layout = new QGridLayout(mGroupOrthomosaic);

    mLabelMethod = new QLabel(this);
    orthomosaic_layout->addWidget(mLabelMethod, 0, 0, 1, 1);
    mComboBoxMethod = new QComboBox(this);
    mComboBoxMethod->addItem("");
    //mComboBoxMethod->addItem("");
    mComboBoxMethod->setItemData(0, "RLB");
    //mComboBoxMethod->setItemData(1, "Seam-based Blending");
    orthomosaic_layout->addWidget(mComboBoxMethod, 0, 1, 1, 1);

    QWidget *widget_orthomosaic = new QWidget();
    mGridLayoutOrthomosaicWidget = new QGridLayout(widget_orthomosaic);
    mGridLayoutOrthomosaicWidget->setContentsMargins(0, 0, 0, 0);
    orthomosaic_layout->addWidget(widget_orthomosaic, 1, 0, 1, 2);

    grid_layout->addWidget(mGroupOrthomosaic, 2, 0, 1, 3);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 3, 0, 1, 3);

    this->retranslate();
    this->clear();
    this->update();
}

void OrthophotoViewImp::initSignalAndSlots()
{
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &OrthophotoView::crs_change);
    connect(mLineEditCRS, &QLineEdit::textChanged, this, &OrthophotoViewImp::update);
    connect(mPushButtonCRS, &QAbstractButton::clicked, this, &OrthophotoView::select_crs);
    connect(mDoubleSpinBoxGSD, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &OrthophotoView::resolutionChanged);
    connect(mDoubleSpinBoxMaxTiltAngle, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &OrthophotoView::maxTiltAngleChanged);
    connect(mComboBoxMethod, &QComboBox::currentIndexChanged, [&](int index) {
        emit orthomosaicMethodChanged(mComboBoxMethod->itemData(index).toString());
        }); // this, &OrthophotoView::orthomosaicMethodChanged);

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &OrthophotoView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void OrthophotoViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("OrthophotoComponent", "Orthophoto"));

    mLabelCRS->setText(QApplication::translate("OrthophotoComponent", "CRS"));

    mGroupOrthoimages->setTitle(QApplication::translate("OrthophotoComponent", "Orthoimages", nullptr));
    mLabelGSD->setText(QApplication::translate("OrthophotoComponent", "Resolution (m):"));
    mLabelmaxTiltAngle->setText(QApplication::translate("OrthophotoComponent", "Maximum tilt angle:"));
    mLabelInterpolation->setText(QApplication::translate("OrthophotoComponent", "Interpolation"));
    mComboBoxInterpolation->setItemText(0, QApplication::translate("OrthophotoComponent", "Nearest"));
    mComboBoxInterpolation->setItemText(1, QApplication::translate("OrthophotoComponent", "Bilinear"));
    mComboBoxInterpolation->setItemText(2, QApplication::translate("OrthophotoComponent", "Bicubic"));

    mGroupOrthomosaic->setTitle(QApplication::translate("OrthophotoComponent", "Orthomosaic", nullptr));
    mLabelMethod->setText(QApplication::translate("OrthophotoComponent", "Method"));
    mComboBoxMethod->setItemText(0, QApplication::translate("OrthophotoComponent", "Robust Local Blending"));
    //mComboBoxMethod->setItemText(1, QApplication::translate("OrthophotoComponent", "Seam-based Blending"));

    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("OrthophotoComponent", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("OrthophotoComponent", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("OrthophotoComponent", "Help"));
}

void OrthophotoViewImp::clear()
{
    setGsd(0.05);
    setMaxTiltAngle(30.);

    const QSignalBlocker blocker_crs(mLineEditCRS);
    mLineEditCRS->clear();

    const QSignalBlocker blocker_method(mComboBoxMethod);
    mComboBoxMethod->setCurrentIndex(0); // Default to "Robust Local Blending"
}

void OrthophotoViewImp::update()
{
    mComboBoxMethod->setVisible(mComboBoxMethod->count() > 1);
    mLabelMethod->setVisible(mComboBoxMethod->count() > 1);
}

auto OrthophotoViewImp::gsd() const -> double
{
    return mDoubleSpinBoxGSD->value();
}

auto OrthophotoViewImp::maxTiltAngle() const -> double
{
    return mDoubleSpinBoxMaxTiltAngle->value();
}

auto OrthophotoViewImp::crs() const -> QString
{
    return mLineEditCRS->text();
}

auto OrthophotoViewImp::interpolation() const -> QString
{
    return mComboBoxInterpolation->currentData().toString();
}

auto OrthophotoViewImp::orthomosaicMethod() const -> QString
{
    return mComboBoxMethod->currentData().toString();
}

void OrthophotoViewImp::setGsd(double gsd)
{
    const QSignalBlocker blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(gsd);
}

void OrthophotoViewImp::setMaxTiltAngle(double maxTiltAngle)
{
    const QSignalBlocker blocker(mDoubleSpinBoxMaxTiltAngle);
    mDoubleSpinBoxMaxTiltAngle->setValue(maxTiltAngle);
}

void OrthophotoViewImp::setCrs(const QString &crs)
{
    mLineEditCRS->setText(crs);
}

void OrthophotoViewImp::setInterpolation(const QString &interpolation)
{
    mComboBoxInterpolation->setCurrentIndex(mComboBoxInterpolation->findData(interpolation));
}

void OrthophotoViewImp::setOrthomosaicMethod(const QString &method)
{
    const QSignalBlocker blocker_method(mComboBoxMethod);
    mComboBoxMethod->setCurrentIndex(mComboBoxMethod->findData(method));

    for (int idx = 0; idx < mGridLayoutOrthomosaicWidget->count(); idx++) {
        QLayoutItem *item = mGridLayoutOrthomosaicWidget->itemAt(idx);
        if (dynamic_cast<QWidgetItem *>(item)) {
            if (item->widget()->objectName().compare(method) == 0)
                item->widget()->setVisible(true);
            else
                item->widget()->setVisible(false);
        }
    }
}

void OrthophotoViewImp::addOrthomosaicMethodWidget(QWidget *orthomosaic)
{
    mComboBoxMethod->addItem(orthomosaic->windowTitle());
    mComboBoxMethod->setItemData(mComboBoxMethod->count() - 1, orthomosaic->objectName());

    mGridLayoutOrthomosaicWidget->addWidget(orthomosaic, 1, 0, 1, 2);
    orthomosaic->setVisible(false);

    update();
}

} // namespace graphos


