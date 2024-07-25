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
#include <QCloseEvent>
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
    QSizePolicy size_policy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    size_policy.setHorizontalStretch(0);
    size_policy.setVerticalStretch(0);
    size_policy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(size_policy);


    QGridLayout *grid_layout = new QGridLayout(this);
    QSpacerItem *horizontal_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    grid_layout->addItem(horizontal_spacer, 3, 0, 1, 1);

    QSpacerItem *vertical_spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);
    grid_layout->addItem(vertical_spacer, 0, 0, 1, 1);

    mLabelStatus = new QLabel(this);
    grid_layout->addWidget(mLabelStatus, 2, 0, 1, 1);

    mPushButtonCancel = new QPushButton(this);
    mPushButtonCancel->setObjectName(QString::fromUtf8("mPushButtonCancel"));
    QSizePolicy size_policy1(QSizePolicy::Maximum, QSizePolicy::Fixed);
    size_policy1.setHorizontalStretch(0);
    size_policy1.setVerticalStretch(0);
    size_policy1.setHeightForWidth(mPushButtonCancel->sizePolicy().hasHeightForWidth());
    mPushButtonCancel->setSizePolicy(size_policy1);
    grid_layout->addWidget(mPushButtonCancel, 3, 3, 1, 1);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setValue(0);
    grid_layout->addWidget(mProgressBar, 1, 0, 1, 4);

    mPushButtonMinimize = new QPushButton(this);
    mPushButtonMinimize->setFocus();
    grid_layout->addWidget(mPushButtonMinimize, 3, 1, 1, 1);

    mPushButtonClose = new QPushButton(this);
    grid_layout->addWidget(mPushButtonClose, 3, 2, 1, 1);

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

//void ProgressBarDialog::changeEvent(QEvent *event)
//{
//    if (event->type() == QEvent::WindowStateChange) {
//        auto state_event = dynamic_cast<QWindowStateChangeEvent *>(event);
//        if (state_event->oldState() != Qt::WindowMinimized && windowState() == Qt::WindowMinimized) {
//            hide();
//            setWindowState(Qt::WindowNoState);
//            event->ignore();
//        }
//    }
//}

void ProgressBarDialog::closeEvent(QCloseEvent *event)
{
    if (mPushButtonClose->isVisible()){
        QDialog::close();
    } else {
        event->ignore();
        onPushButtonCancelClicked();
    }
}

void ProgressBarDialog::setStatusText(const QString& text)
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
