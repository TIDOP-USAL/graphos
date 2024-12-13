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
    this->resize(380, 250);

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

    mLabelGSD = new QLabel(this);
    grid_layout->addWidget(mLabelGSD, 1, 0, 1, 1);
    mDoubleSpinBoxGSD = new QDoubleSpinBox(this);
    mDoubleSpinBoxGSD->setSingleStep(0.1);
    grid_layout->addWidget(mDoubleSpinBoxGSD, 1, 1, 1, 2);

    mLabelInterpolation = new QLabel(this);
    grid_layout->addWidget(mLabelInterpolation, 2, 0, 1, 1);
    mComboBoxInterpolation = new QComboBox(this);
    mComboBoxInterpolation->addItem("");
    mComboBoxInterpolation->addItem("");
    mComboBoxInterpolation->addItem("");
    mComboBoxInterpolation->setItemData(0, "NEAREST");
    mComboBoxInterpolation->setItemData(1, "BILINEAR");
    mComboBoxInterpolation->setItemData(2, "BICUBIC");
    grid_layout->addWidget(mComboBoxInterpolation, 2, 1, 1, 2);

    mButtonBox = new QDialogButtonBox(this);
    mButtonBox->setOrientation(Qt::Orientation::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Apply | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    grid_layout->addWidget(mButtonBox, 7, 0, 1, 3);

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

    connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mButtonBox->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, this, &OrthophotoView::run);
    connect(mButtonBox->button(QDialogButtonBox::Help), &QAbstractButton::clicked, this, &DialogView::help);
}

void OrthophotoViewImp::retranslate()
{
    this->setWindowTitle(QApplication::translate("OrthophotoView", "Orthophoto"));

    mLabelGSD->setText(QApplication::translate("OrthophotoView", "Resolution (m):"));
    mLabelCRS->setText(QApplication::translate("OrthophotoView", "CRS"));
    mLabelInterpolation->setText(QApplication::translate("OrthophotoView", "Interpolation"));
    mComboBoxInterpolation->setItemText(0, QApplication::translate("OrthophotoView", "Nearest"));
    mComboBoxInterpolation->setItemText(1, QApplication::translate("OrthophotoView", "Bilinear"));
    mComboBoxInterpolation->setItemText(2, QApplication::translate("OrthophotoView", "Bicubic"));

    mButtonBox->button(QDialogButtonBox::Cancel)->setText(QApplication::translate("BilateralFilterView", "Cancel"));
    mButtonBox->button(QDialogButtonBox::Apply)->setText(QApplication::translate("BilateralFilterView", "Run"));
    mButtonBox->button(QDialogButtonBox::Help)->setText(QApplication::translate("BilateralFilterView", "Help"));
}

void OrthophotoViewImp::clear()
{
    const QSignalBlocker signal_blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(0.05);

    const QSignalBlocker blocker_crs(mLineEditCRS);
    mLineEditCRS->clear();
}

void OrthophotoViewImp::update()
{}

auto OrthophotoViewImp::gsd() const -> double
{
    return mDoubleSpinBoxGSD->value();
}

auto OrthophotoViewImp::crs() const -> QString
{
    return mLineEditCRS->text();
}

auto OrthophotoViewImp::interpolation() const -> QString
{
    return mComboBoxInterpolation->currentData().toString();
}

void OrthophotoViewImp::setGsd(double gsd)
{
    const QSignalBlocker blocker(mDoubleSpinBoxGSD);
    mDoubleSpinBoxGSD->setValue(gsd);
}

void OrthophotoViewImp::setCrs(const QString &crs)
{
    mLineEditCRS->setText(crs);
}

void OrthophotoViewImp::setInterpolation(const QString &interpolation)
{
    mComboBoxInterpolation->setCurrentIndex(mComboBoxInterpolation->findData(interpolation));
}


} // namespace graphos


