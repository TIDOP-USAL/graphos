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

#ifndef GRAPHOS_ORIENTATION_MODEL_H
#define GRAPHOS_ORIENTATION_MODEL_H

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

    bool calibratedCamera() const override;
    void setSparseModel(const tl::Path &sparseModel) override;
    void setOffset(const tl::Path &offset) override;
    void setGroundPoints(const tl::Path &groundPoints) override;
    bool isPhotoOriented(size_t imageId) const override;
    CameraPose photoOrientation(size_t imageId) const override;
    void addPhotoOrientation(size_t imageId,
                             const CameraPose &orientation) override;
    tl::Path database() const override;
    tl::Path projectFolder() const override;
    bool gpsPositions() const override;
    bool rtkOrientations() const override;
    tl::Path reconstructionPath() const override;
    TL_DEPRECATED("", "2.0")
    void setReconstructionPath(const tl::Path &reconstructionPath) override;
    void clearProject() override;
    const std::map<int, Camera> &cameras() const override;
    bool updateCamera(int id, const Camera &camera) override;
    const std::unordered_map<size_t, Image> &images() const override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_MODEL_H
