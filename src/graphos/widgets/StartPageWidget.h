/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#ifndef GRAPHOS_START_PAGE_WIDGET_H
#define GRAPHOS_START_PAGE_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QLabel;
class QSplitter;
class QCommandLinkButton;
class QListWidget;

namespace graphos
{

class StartPageWidget
  : public GraphosWidgetView
{

  Q_OBJECT

public:

  StartPageWidget(QWidget *parent = nullptr);
  ~StartPageWidget() override = default;

  void setHistory(const QStringList &history);

// GraphosWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

signals:

  void openNew();
  void openProject();
  void openSettings();
  void clearHistory();
  void openProjectFromHistory(QString);

// QWidget interface

protected:

  void changeEvent(QEvent *event) override;

protected:

  QLabel *mLabelGraphosLogo;
  QLabel *mLabelRecentProjects;
  QSplitter *mSplitter;
  QCommandLinkButton *mCommandLinkButtonNewProject;
  QCommandLinkButton *mCommandLinkButtonOpenProject;
  QCommandLinkButton *mCommandLinkButtonSettings;
  QListWidget *mListWidgetRecentProjects;
  QCommandLinkButton *mCommandLinkButtonClearHistory;
};

} // namespace graphos

#endif // GRAPHOS_START_PAGE_WIDGET_H
