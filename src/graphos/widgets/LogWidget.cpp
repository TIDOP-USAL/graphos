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

#include "LogWidget.h"

#include <tidop/core/chrono.h>

TL_DISABLE_WARNINGS
#include <QToolBar>
#include <QGridLayout>
#include <QListWidgetItem>
#include <QAbstractItemModel>
#include <QApplication>
TL_DEFAULT_WARNINGS

using namespace tl;

namespace graphos
{

EnumFlags<MessageLevel> LogWidget::sLevel = EnumFlags<MessageLevel>(MessageLevel::all);
EnumFlags<MessageLevel> LogWidget::sFilterLevel = EnumFlags<MessageLevel>(MessageLevel::all);
std::mutex LogWidget::mtx;

LogWidget::LogWidget(QWidget *parent)
  : GraphosWidgetView(parent),
    mGridLayout(new QGridLayout(this))
{
    LogWidget::initUI();
    LogWidget::initSignalAndSlots();
    LogWidget::retranslate();
}

LogWidget::~LogWidget()
{

}

void LogWidget::filter(tl::MessageLevel level)
{
    sFilterLevel = level;

    mMsgErrorAction->setChecked(sFilterLevel.isEnabled(MessageLevel::error));
    mMsgWarningAction->setChecked(sFilterLevel.isEnabled(MessageLevel::warning));
    mMsgInfoAction->setChecked(sFilterLevel.isEnabled(MessageLevel::info));

    refresh();
}

void LogWidget::setLogLevel(MessageLevel level)
{
    sLevel = level;
}

//void LogWidget::print(const std::string &msg, const std::string &date, MessageLevel level)
//{
//    std::lock_guard<std::mutex> lck(LogWidget::mtx);
//
//    QString _msg = QString(date.c_str()) + " " + msg.c_str();
//
//    QListWidgetItem *qListWidgetItem = new QListWidgetItem(_msg);
//    qListWidgetItem->setData(Qt::UserRole, QVariant(static_cast<int>(level)));
//
//    if (level == MessageLevel::error)
//        qListWidgetItem->setForeground(Qt::red);
//    else if (level == MessageLevel::warning)
//        qListWidgetItem->setForeground(Qt::magenta);
//
//    mListWidget->insertItem(mListWidget->count(), qListWidgetItem);
//
//    if (!sFilterLevel.isEnabled(level)) {
//        mListWidget->setRowHidden(mListWidget->count() - 1, true);
//    }
//}

void LogWidget::print(String msg, tl::MessageLevel level) const
{
    std::lock_guard<std::mutex> lck(LogWidget::mtx);

    auto date = tl::formatTimeToString("%d/%b/%Y %H:%M:%S");

    //QString _msg = QString(date.c_str()) + " " + msg.c_str();
    std::stringstream ss;
    ss << date << " " << msg;
    std::string message = ss.str();

    QListWidgetItem *qListWidgetItem = new QListWidgetItem(message.c_str());
    qListWidgetItem->setData(Qt::UserRole, QVariant(static_cast<int>(level)));

    if (level == MessageLevel::error)
        qListWidgetItem->setForeground(Qt::red);
    else if (level == MessageLevel::warning)
        qListWidgetItem->setForeground(Qt::magenta);
    else if (level == MessageLevel::success)
        qListWidgetItem->setForeground(Qt::darkGreen);

    mListWidget->insertItem(mListWidget->count(), qListWidgetItem);

    if (!sFilterLevel.isEnabled(level)) {
        mListWidget->setRowHidden(mListWidget->count() - 1, true);
    }
}

void LogWidget::refresh()
{
    std::lock_guard<std::mutex> lck(LogWidget::mtx);
    MessageLevel level;
    for (int i = 0; i < mListWidget->count(); i++) {
        QListWidgetItem *qListWidgetItem = mListWidget->item(i);
        level = static_cast<tl::MessageLevel>(qListWidgetItem->data(Qt::UserRole).toInt());
        mListWidget->setRowHidden(i, !sFilterLevel.isEnabled(level));
    }
}

void LogWidget::onPushButtonShowLogWarningToggled(bool active)
{
    sFilterLevel.activeFlag(MessageLevel::warning, active);
    refresh();
}

void LogWidget::onPushButtonShowLogErrorsToggled(bool active)
{
    sFilterLevel.activeFlag(MessageLevel::error, active);
    refresh();
}

void LogWidget::onPushButtonShowLogInfoToggled(bool active)
{
    sFilterLevel.activeFlag(MessageLevel::info, active);
    refresh();
}

void LogWidget::onPushButtonShowLogDebugToggled(bool active)
{
    sFilterLevel.activeFlag(MessageLevel::debug, active);
    refresh();
}

void LogWidget::onRowsInserted(const QModelIndex &parent, int start, int end, LogWidget::QPrivateSignal)
{
    update();
}

void LogWidget::onRowsRemoved(const QModelIndex &parent, int start, int end, LogWidget::QPrivateSignal)
{
    update();
}

void LogWidget::update()
{
    mClearAction->setEnabled(mListWidget->count() > 0);
}

void LogWidget::retranslate()
{
    mMsgErrorAction->setText(QApplication::translate("LogWidget", "Show errors"));
    mMsgErrorAction->setStatusTip(QApplication::translate("LogWidget", "Show errors"));
    mMsgWarningAction->setText(QApplication::translate("LogWidget", "Show warnings"));
    mMsgWarningAction->setStatusTip(QApplication::translate("LogWidget", "Show warnings"));
    mMsgInfoAction->setText(QApplication::translate("LogWidget", "Show messages"));
    mMsgInfoAction->setStatusTip(QApplication::translate("LogWidget", "Show messages"));
    mClearAction->setText(QApplication::translate("LogWidget", "Clean log"));
    mClearAction->setStatusTip(QApplication::translate("LogWidget", "Clean log"));
}

void LogWidget::clear()
{
    std::lock_guard<std::mutex> lck(LogWidget::mtx);
    mListWidget->clear();
    update();
}

void LogWidget::initUI()
{
    QToolBar *toolBar = new QToolBar(this);

    mMsgErrorAction = new QAction(this);
    mMsgErrorAction->setIcon(QIcon::fromTheme("show-errors"));
    mMsgErrorAction->setCheckable(true);
    mMsgErrorAction->setChecked(true);
    toolBar->addAction(mMsgErrorAction);

    mMsgWarningAction = new QAction(this);
    mMsgWarningAction->setIcon(QIcon::fromTheme("show-warnings"));
    mMsgWarningAction->setCheckable(true);
    mMsgWarningAction->setChecked(true);
    toolBar->addAction(mMsgWarningAction);

    mMsgInfoAction = new QAction(this);
    mMsgInfoAction->setIcon(QIcon::fromTheme("show-info"));
    mMsgInfoAction->setCheckable(true);
    mMsgInfoAction->setChecked(true);
    toolBar->addAction(mMsgInfoAction);

    toolBar->addSeparator();

    mClearAction = new QAction(this);
    mClearAction->setIcon(QIcon::fromTheme("clean-console"));
    mClearAction->setStatusTip(tr("Clean log"));
    toolBar->addAction(mClearAction);

    mGridLayout->setMargin(0);
    mGridLayout->addWidget(toolBar);
    mListWidget = new QListWidget(this);
    mGridLayout->addWidget(mListWidget);

    LogWidget::update();
}

void LogWidget::initSignalAndSlots()
{
    connect(mMsgErrorAction, SIGNAL(toggled(bool)), this, SLOT(onPushButtonShowLogErrorsToggled(bool)));
    connect(mMsgWarningAction, SIGNAL(toggled(bool)), this, SLOT(onPushButtonShowLogWarningToggled(bool)));
    connect(mMsgInfoAction, SIGNAL(toggled(bool)), this, SLOT(onPushButtonShowLogInfoToggled(bool)));
    connect(mClearAction, SIGNAL(triggered(bool)), this, SLOT(clear()));
    connect(mListWidget->model(), SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(onRowsInserted(const QModelIndex &, int, int)));
    connect(mListWidget->model(), SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(onRowsRemoved(const QModelIndex &, int, int)));
}

void LogWidget::debug(String message)
{
    if (sLevel.isEnabled(MessageLevel::debug)) {
        print(message, MessageLevel::debug);
    }
}

void LogWidget::info(String message)
{
    if (sLevel.isEnabled(MessageLevel::info)) {
        print(message, MessageLevel::info);
    }
}

void LogWidget::success(String message)
{
    if (sLevel.isEnabled(MessageLevel::success)) {
        print(message, MessageLevel::success);
    }
}

void LogWidget::warning(String message)
{
    if (sLevel.isEnabled(MessageLevel::warning)) {
        print(message, MessageLevel::warning);
    }
}

void LogWidget::error(String message)
{
    if (sLevel.isEnabled(MessageLevel::error)) {
        print(message, MessageLevel::error);
    }
}

} // namespace graphos