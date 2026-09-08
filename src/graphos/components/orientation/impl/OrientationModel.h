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

#include "graphos/components/orientation/OrientationModel.h"

namespace graphos
{

class Project;

class OrientationModelImp
  : public OrientationModel
{

public:

    OrientationModelImp(Project *project,
                        QObject *parent = nullptr);
    ~OrientationModelImp() override = default;

// OrientationModel interface

public:

    auto orientationMethod() const -> std::string override;
    auto hasPriorCalibration() const -> bool override;
    auto hasAdjustedCalibration() const -> bool override;
    void setSparseModel(const tl::Path &sparseModel) override;
    void setEnuCrs(const QString &enuCrs) override;
    void setGroundPoints(const tl::Path &groundPoints) override;
    void setCameraPosesFile(const tl::Path &cameraPosesFile) override;
    auto isPhotoOriented(size_t imageId) const -> bool override;
    auto photoOrientation(size_t imageId) const -> CameraPose override;
    void addPhotoOrientation(size_t imageId,
                             const CameraPose &orientation) override;
    auto database() const -> tl::Path override;
    auto projectFolder() const -> tl::Path override;
    auto hasCameraPoses() const -> bool override;
    auto hasRtkPoses() const -> bool override;
    auto hasGroundControlPoints() const -> bool override;
    auto groundControlPointsFile() const -> tl::Path override;
    auto existReconstruction() const -> bool override;
    void clearProject() override;
    auto cameras() -> CameraRepository & override;
    auto updateCamera(int id, const Camera& camera) -> bool override;
    auto images() const -> const ImageRepository & override;
    auto orientationReport() const -> OrientationReport override;
    void setOrientationReport(const OrientationReport &orientationReport) override;
    void setOrientationConfig(std::shared_ptr<OrientationProperties> config) override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;

};

} // namespace graphos
