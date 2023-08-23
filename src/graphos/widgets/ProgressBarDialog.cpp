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

#include "ProgressBarDialog.h"

TL_DISABLE_WARNINGS
#include <QPushButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
TL_DEFAULT_WARNINGS

namespace graphos
{

ProgressBarDialog::ProgressBarDialog(QWidget *parent)
  : DialogView(parent, Qt::WindowStaysOnTopHint),
    mAutoClose(false)
{
    ProgressBarDialog::initUI();
    ProgressBarDialog::initSignalAndSlots();
}

void ProgressBarDialog::initUI()
{
    this->setObjectName(QString::fromUtf8("ProgressBarView"));
    this->resize(445, 121);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);


    QGridLayout *gridLayout = new QGridLayout(this);
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    gridLayout->addItem(horizontalSpacer, 3, 0, 1, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);
    gridLayout->addItem(verticalSpacer, 0, 0, 1, 1);

    mLabelStatus = new QLabel(this);
    gridLayout->addWidget(mLabelStatus, 2, 0, 1, 1);

    mPushButtonCancel = new QPushButton(this);
    mPushButtonCancel->setObjectName(QString::fromUtf8("mPushButtonCancel"));
    QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(mPushButtonCancel->sizePolicy().hasHeightForWidth());
    mPushButtonCancel->setSizePolicy(sizePolicy1);
    gridLayout->addWidget(mPushButtonCancel, 3, 3, 1, 1);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setValue(0);
    gridLayout->addWidget(mProgressBar, 1, 0, 1, 4);

    mPushButtonMinimize = new QPushButton(this);
    mPushButtonMinimize->setFocus();
    gridLayout->addWidget(mPushButtonMinimize, 3, 1, 1, 1);

    mPushButtonClose = new QPushButton(this);
    gridLayout->addWidget(mPushButtonClose, 3, 2, 1, 1);

    ProgressBarDialog::retranslate();
    ProgressBarDialog::update();

    mPushButtonMinimize->setEnabled(false);
}

void ProgressBarDialog::initSignalAndSlots()
{
    connect(mPushButtonMinimize, &QPushButton::clicked, this, &ProgressBarDialog::onMinimized);
    connect(mPushButtonCancel, &QPushButton::clicked, this, &ProgressBarDialog::onPushButtonCancelClicked);
    connect(mPushButtonClose, &QPushButton::clicked, this, &ProgressBarDialog::close);
}

void ProgressBarDialog::clear()
{

}

void ProgressBarDialog::update()
{

}

void ProgressBarDialog::retranslate()
{
    this->setWindowTitle(QCoreApplication::translate("ProgressBarView", "Progress Bar", nullptr));
    mLabelStatus->setText(QCoreApplication::translate("ProgressBarView", "TextLabel", nullptr));
    mPushButtonCancel->setText(QCoreApplication::translate("ProgressBarView", "Cancel", nullptr));
    mPushButtonMinimize->setText(QCoreApplication::translate("ProgressBarView", "Minimize", nullptr));
    mPushButtonClose->setText(QCoreApplication::translate("ProgressBarView", "Close", nullptr));
}

void ProgressBarDialog::setStatusText(QString text)
{
    mLabelStatus->setText(text);
}

void ProgressBarDialog::setCloseAuto(bool active)
{
    mAutoClose = active;
}

void ProgressBarDialog::setRange(int min, int max)
{
    mProgressBar->setRange(min, max);
}

void ProgressBarDialog::setValue(int value)
{
    mProgressBar->setValue(value);
}

void ProgressBarDialog::setInitialized()
{
    mPushButtonCancel->setVisible(true);
    mPushButtonCancel->setEnabled(true);
    mPushButtonClose->setVisible(false);
    mPushButtonMinimize->setEnabled(true);

    this->show();
}

void ProgressBarDialog::setFinished()
{
    mPushButtonCancel->setVisible(false);
    mPushButtonClose->setVisible(true);
    mPushButtonClose->setText(tr("Close"));

    mProgressBar->setValue(mProgressBar->maximum());
    if (!this->isVisible() && !mAutoClose) {
        this->show();
    } else if (mAutoClose) {
        this->close();
    }
    mPushButtonMinimize->setDisabled(true);
}

void ProgressBarDialog::setTitle(QString title)
{
    mProgressBar->setWindowTitle(title);
}

void ProgressBarDialog::onMinimized()
{
    this->hide();
}

void ProgressBarDialog::onPushButtonCancelClicked()
{
    mPushButtonCancel->setEnabled(false);
    emit cancel();
}

} // namespace graphos
