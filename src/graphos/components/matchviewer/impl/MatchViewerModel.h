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

#ifndef GRAPHOS_MATCH_VIEWER_MODEL_H
#define GRAPHOS_MATCH_VIEWER_MODEL_H

#include <memory>

#include "graphos/components/matchviewer/MatchViewerModel.h"

class QSettings;

namespace graphos
{

class Project;

class MatchViewerModelImp
  : public MatchViewerModel
{

    Q_OBJECT

public:

    MatchViewerModelImp(Project *project,
                        QObject *parent = nullptr);
    ~MatchViewerModelImp() override;

// MatchViewerModel interface

public:

    auto backgroundColor() const -> QString override;
    auto markerType() const -> int override;
    auto markerSize() const -> int override;
    auto markerWidth() const -> int override;
    auto markerColor() const -> QString override;
    auto viewerSelectMarkerWidth() const -> int override;
    auto selectedMarkerColor() const -> QString override;
    auto lineColor() const -> QString override;
    auto lineWidth() const -> int override;

    auto images() const -> const Images& override;
    auto image(size_t imageId) const -> Image override;
    auto imagePairs(size_t imageId) const -> std::vector<size_t> override;
    auto loadMatches(size_t imageId1, size_t imageId2) const -> Matches override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
    QSettings *mSettings;
};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_MODEL_H
