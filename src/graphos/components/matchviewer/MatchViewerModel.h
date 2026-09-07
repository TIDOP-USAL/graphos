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

#include "graphos/framework/mvp/Model.h"
#include "graphos/core/image/Image.h"
#include "graphos/core/image/ImageRepository.h"

namespace graphos
{

class MatchViewerModel
  : public Model
{

    Q_OBJECT

public:

    using Matches = std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>>;

public:

    MatchViewerModel(QObject *parent = nullptr) : Model(parent) {}
    ~MatchViewerModel() override = default;

    virtual auto backgroundColor() const -> QString = 0;
    virtual auto markerType() const -> int = 0;
    virtual auto markerSize() const -> int = 0;
    virtual auto markerWidth() const -> int = 0;
    virtual auto markerColor() const -> QString = 0;
    virtual auto viewerSelectMarkerWidth() const -> int = 0;
    virtual auto selectedMarkerColor() const -> QString = 0;
    virtual auto lineColor() const -> QString = 0;
    virtual auto lineWidth() const -> int = 0;

    virtual auto images() const -> const ImageRepository & = 0;
    virtual auto findImage(size_t imageId) const -> const Image * = 0;
    virtual auto imagePairs(size_t imageId) const -> std::vector<size_t> = 0;
    virtual auto loadMatches(size_t imageId1, size_t imageId2) const -> Matches = 0;


};

} // namespace graphos
