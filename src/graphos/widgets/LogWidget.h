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

#ifndef GRAPHOS_CONSOLE_WIDGET_H
#define GRAPHOS_CONSOLE_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

#include <mutex>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>
#include <tidop/core/msg/handler.h>
#include <tidop/core/log.h>

class QDateTimeEdit;
class QListWidget;
class QGridLayout;

namespace graphos
{

class LogWidget
  : public GraphosWidgetView,
    public tl::MessageHandler
{
    Q_OBJECT

public:

    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget() override;

    /*!
     * \brief Filter the messages displayed in the console
     * \param[in] level Log level
     */
    void filter(tl::MessageLevel level);

    /*!
     * \brief Set log level
     * \param[in] level Log level
     */
    void setLogLevel(tl::MessageLevel level);

    //void print(const std::string &msg, const std::string &date, tl::MessageLevel level);
    void print(String msg, tl::MessageLevel level);
    void refresh();

private slots:

    void onPushButtonShowLogWarningToggled(bool active);
    void onPushButtonShowLogErrorsToggled(bool active);
    void onPushButtonShowLogInfoToggled(bool active);
    void onPushButtonShowLogDebugToggled(bool active);
    void onRowsInserted(const QModelIndex &parent, int start, int end, QPrivateSignal);
    void onRowsRemoved(const QModelIndex &parent, int start, int end, QPrivateSignal);

// GraphosWidgetView interface

protected slots:

    void update() override;
    void retranslate() override;

public slots:

    void clear() override;

private:

    void initUI() override;
    void initSignalAndSlots() override;

// MessageHandler

public:

    void debug(String message) override;
    void info(String message) override;
    void success(String message) override;
    void warning(String message) override;
    void error(String message) override;

//public:
//
//    void onMsgDebug(const std::string &msg, const std::string &date) override;
//    void onMsgInfo(const std::string &msg, const std::string &date) override;
//    void onMsgWarning(const std::string &msg, const std::string &date) override;
//    void onMsgError(const std::string &msg, const std::string &date) override;

private:

    static tl::EnumFlags<tl::MessageLevel> sLevel;
    static tl::EnumFlags<tl::MessageLevel> sFilterLevel;
    static std::mutex mtx;

    QGridLayout *mGridLayout;
    QListWidget *mListWidget;
    QAction *mMsgErrorAction;
    QAction *mMsgWarningAction;
    QAction *mMsgInfoAction;
    QAction *mClearAction;

};

} // namespace graphos

#endif // GRAPHOS_CONSOLE_WIDGET_H
