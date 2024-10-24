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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_MODEL_INTERFACE_H
#define GRAPHOS_FEATURE_EXTRACTOR_MODEL_INTERFACE_H

#include <unordered_map>

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"
#include "graphos/core/image.h"
#include "graphos/core/features/features.h"

namespace graphos
{

class Feature;
class Camera;

class FeatureExtractorModel
  : public Model
{

public:

    FeatureExtractorModel(QObject *parent = nullptr) : Model(parent) {}
    ~FeatureExtractorModel() override = default;

    virtual auto featureExtractor() const -> std::shared_ptr<Feature> = 0;
    virtual void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) = 0;
    virtual void setFeatureExtractorReport(const FeatureExtractorReport &report) = 0;
    virtual auto database() const -> tl::Path = 0;
    virtual void addFeatures(size_t imageId, const QString &featuresFile) = 0;
    virtual auto useCuda() const -> bool = 0;
    virtual auto images() const -> const std::unordered_map<size_t, Image>& = 0;
    virtual auto cameras() const -> const std::map<int, Camera>& = 0;
    virtual void clearProject() = 0;

};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_MODEL_INTERFACE_H
