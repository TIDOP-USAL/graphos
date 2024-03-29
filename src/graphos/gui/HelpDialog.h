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

#ifndef GRAPHOS_HELP_DIALOG_H
#define GRAPHOS_HELP_DIALOG_H

#include "graphos/core/mvp.h"

#include <QTextBrowser>

class QHelpEngine;

class HelpBrowser
  : public QTextBrowser
{

public:

    HelpBrowser(QHelpEngine *helpEngine,
                QWidget *parent = nullptr);

    QVariant loadResource(int type, const QUrl &name);


private:

    QHelpEngine *mHelpEngine;
};




namespace graphos
{

class HelpDialog
  : public QDialog
{

    Q_OBJECT

public:

    HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog() override;

public slots:

    /*!
     * \brief Pagina inicial
     */
    void navigateHome();

    /*!
     * \brief Establece la página de ayuda
     * \param[in] page Página de ayuda
     */
    void setPage(const QString &page);

private slots:


    void navigateBackward();
    void navigateForward();
    void setContentSource(const QModelIndex &index);
    void setIndexSource(const QModelIndex &index);
    void searchInvoked();
    void searchFinished(int hits);

// DialogView interface

private:

    void init();

private slots:

    void retranslate();

private:

    QHelpEngine *mHelpEngine;
    HelpBrowser *mHelpBrowser;
    QTextBrowser *mSearchResultsWidget;

    QAction *mNavigateHomeAction;
    QAction *mNavigateBackwardAction;
    QAction *mNavigateForwardAction;
};

} // namespace graphos


#endif // GRAPHOS_HELP_DIALOG_H
