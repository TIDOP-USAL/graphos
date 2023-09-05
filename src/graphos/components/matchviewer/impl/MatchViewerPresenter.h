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

#ifndef GRAPHOS_MATCH_VIEWER_PRESENTER_H
#define GRAPHOS_MATCH_VIEWER_PRESENTER_H

#include <memory>

#include <QObject>

#include "graphos/components/matchviewer/MatchViewerPresenter.h"

namespace graphos
{

class MatchViewerView;
class MatchViewerModel;
class SettingsModel;


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

    void setLeftImage(size_t imageId) override;
    void setRightImage(size_t imageId) override;

protected slots:

    void loadMatches(size_t imageId1,
                     size_t imageId2) override;

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

public slots:

private:

    MatchViewerView *mView;
    MatchViewerModel *mModel;

};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_PRESENTER_H
