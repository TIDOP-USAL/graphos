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

#ifndef GRAPHOS_FEATURE_MATCHING_MODEL_H
#define GRAPHOS_FEATURE_MATCHING_MODEL_H

#include <QObject>

#include "graphos/components/featmatch/FeatureMatchingModel.h"
#include "graphos/core/project.h"

class QSettings;

namespace graphos
{

class FeatureMatchingModelImp
  : public FeatureMatchingModel
{

public:

    FeatureMatchingModelImp(Project *project,
                            QObject *parent = nullptr);
    ~FeatureMatchingModelImp() override;

// FeatureMatchingModel interface

public:

    auto featureMatching() const -> std::shared_ptr<FeatureMatching> override;
    void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) override;
    void setFeatureMatchingReport(const FeatureMatchingReport& report) override;
    auto database() const -> tl::Path override;
    auto useCuda() const -> bool override;
    auto spatialMatching() const -> bool override;
    auto geodeticCoordinates() const -> bool override;
    void writeMatchPairs() override;
    void cleanProject() override;
    auto imagesSize() const -> size_t override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
    QSettings *mSettings;
};

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_MODEL_H
