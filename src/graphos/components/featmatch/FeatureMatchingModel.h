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

#ifndef GRAPHOS_FEATURE_MATCHING_MODEL_INTERFACE_H
#define GRAPHOS_FEATURE_MATCHING_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"
#include "graphos/core/features/features.h"

#include <tidop/core/path.h>

namespace graphos
{

class FeatureMatching;

class FeatureMatchingModel
  : public Model
{

    Q_OBJECT

public:

    FeatureMatchingModel(QObject *parent = nullptr) : Model(parent) {}
    ~FeatureMatchingModel() override = default;

    virtual auto featureMatching() const -> std::shared_ptr<FeatureMatching> = 0;
    virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;
    virtual void setFeatureMatchingReport(const FeatureMatchingReport &report) = 0;
    virtual auto database() const -> tl::Path = 0;
    virtual auto useCuda() const -> bool = 0;
    virtual auto spatialMatching() const -> bool = 0;
    virtual auto geodeticCoordinates() const -> bool = 0;
    virtual void writeMatchPairs() = 0;
    virtual auto imagesSize() const -> size_t = 0;
    virtual void cleanProject() = 0;
};

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_MODEL_INTERFACE_H
