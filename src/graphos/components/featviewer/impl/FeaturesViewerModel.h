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

#ifndef GRAPHOS_FEATURES_VIEWER_MODEL_H
#define GRAPHOS_FEATURES_VIEWER_MODEL_H

#include <memory>

#include "graphos/components/featviewer/FeaturesViewerModel.h"

class QSettings;

namespace graphos
{

class Project;

class FeaturesViewerModelImp
  : public FeaturesViewerModel
{

    Q_OBJECT

public:

    FeaturesViewerModelImp(Project *project,
                           QObject *parent = nullptr);
    ~FeaturesViewerModelImp() override;

// FeaturesViewerModel interface

public:

    auto images() const -> const std::unordered_map<size_t, Image>& override;
    auto image(size_t imageId) const -> Image override;
    auto loadKeypoints(size_t imageId) -> std::vector<std::tuple<QPointF, float, float>> override;

    auto backgroundColor() const -> QString override;
    auto markerType() const -> int override;
    auto markerSize() const -> int override;
    auto markerWidth() const -> int override;
    auto markerColor() const -> QString override;
    auto selectedMarkerWidth() const -> int override;
    auto selectedMarkerColor() const -> QString override;

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

#endif // GRAPHOS_FEATURES_VIEWER_MODEL_H
