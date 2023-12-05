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

#ifndef GRAPHOS_FEATURE_EXTRACTOR_MODEL_H
#define GRAPHOS_FEATURE_EXTRACTOR_MODEL_H

#include "graphos/components/featextract/FeatureExtractorModel.h"

class QSettings;

namespace graphos
{

class Project;
class Feature;

class FeatureExtractorModelImp final
  : public FeatureExtractorModel
{

public:

    FeatureExtractorModelImp(Project *project,
                             QObject *parent = nullptr);
    ~FeatureExtractorModelImp() override;

// FeatureExtractorModel interface

public:

    std::shared_ptr<Feature> featureExtractor() const override;
    void setFeatureExtractor(const std::shared_ptr<Feature> &featureExtractor) override;
    tl::Path database() const override;
    void addFeatures(size_t imageId, const QString &featuresFile) override;
    bool useCuda() const override;
    const std::unordered_map<size_t, Image> &images() const override;
    const std::map<int, Camera> &cameras() const override;
    void clearProject() override;

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

#endif // GRAPHOS_FEATURE_EXTRACTOR_MODEL_H
