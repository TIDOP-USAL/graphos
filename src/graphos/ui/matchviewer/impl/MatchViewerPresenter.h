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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_MATCH_VIEWER_PRESENTER_H
#define GRAPHOS_MATCH_VIEWER_PRESENTER_H

#include <memory>

#include <QObject>

#include "graphos/ui/matchviewer/MatchViewerPresenter.h"

namespace graphos
{

class MatchViewerView;
class MatchViewerModel;
class SettingsModel;
class HelpDialog;


class MatchViewerPresenterImp
  : public MatchViewerPresenter
{
  Q_OBJECT

public:

  MatchViewerPresenterImp(MatchViewerView *view,
                          MatchViewerModel *model);

  ~MatchViewerPresenterImp() override;

// MatchViewerPresenter interface

public slots:

  void setLeftImage(const QString &image) override;
  void setRightImage(const QString &image) override;

protected slots:

  void loadMatches(const QString &imageLeft,
                   const QString &imageRight) override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

public slots:

// MatchViewerPresenter interface


protected slots:

private:

  MatchViewerView *mView;
  MatchViewerModel *mModel;
  HelpDialog *mHelp;

};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_PRESENTER_H