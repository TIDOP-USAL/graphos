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

#include <QObject>

#include "graphos/components/dense/DensificationModel.h"

namespace graphos
{

class Project;

class DensificationModelImp
  : public DensificationModel
{
    Q_OBJECT

public:

    DensificationModelImp(Project *project,
                          QObject *parent = nullptr);
    ~DensificationModelImp() override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

// DensificationModel interface

public:

    auto densificationConfig() const -> std::shared_ptr<DensificationProperties> override;
    auto projectFolder() const -> tl::Path override;
    auto database() const -> tl::Path override;
    auto useCuda() const -> bool override;
    auto existDenseModel() const -> bool override;
    auto images() const -> const ImageRepository & override;
    auto cameras() const -> const CameraRepository & override;
    auto cameraPoses() const -> const CameraPosesRepository & override;
    auto groundPoints() const -> std::vector<GroundPoint> override;
    void cleanProject() override;

public slots:

    void setDensificationConfig(std::shared_ptr<DensificationProperties> config) override;
    void setDenseModel(tl::Path denseModel) override;
    void setDensificationReport(DensificationReport report) override;

protected:

    Project *mProject;

};

} // End namespace graphos