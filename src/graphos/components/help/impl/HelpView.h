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

#pragma once

#include "graphos/components/help/HelpView.h"

#include <QTextBrowser>
#include <QHelpEngine>
#include <QHelpContentWidget>
#include <QHelpIndexWidget>
#include <QHelpSearchQueryWidget>


class QHelpEngine;

namespace graphos
{

class HelpBrowser
  : public QTextBrowser
{
    Q_OBJECT

public:

    HelpBrowser(QHelpEngine *helpEngine,
                QWidget *parent = nullptr);

    QVariant loadResource(int type, const QUrl &name) override;

private slots:

    void setDocumentActivated(const QHelpLink &link, const QString &s);

private:

    QHelpEngine *mHelpEngine;
};


class HelpViewImp
  : public HelpView
{

    Q_OBJECT

public:

    HelpViewImp(QWidget *parent = nullptr);
    ~HelpViewImp() override;

// HelpView

public slots:

    void navigateHome() override;
    void navigateBackward() override;
    void navigateForward() override;
    void setUrlBase(const QString &urlBase) override;
    void setHomePage(const QString &homePage) override;
    void setHelpPage(const QString &helpPage) override;
    void searchInvoked() override;
    void searchFinished(int hits) override;

// DialogView

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

protected:

    QHelpEngine *mHelpEngine;
    HelpBrowser *mHelpBrowser;
    QTextBrowser *mSearchResultsWidget;

    QAction *mNavigateHomeAction;
    QAction *mNavigateBackwardAction;
    QAction *mNavigateForwardAction;

    QString mUrlBase;
    QUrl mHomePage;
};

} // namespace graphos
