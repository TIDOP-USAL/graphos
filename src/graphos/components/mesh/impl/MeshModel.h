/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 
#ifndef GRAPHOS_MESH_MODEL_H
#define GRAPHOS_MESH_MODEL_H

#include "graphos/components/mesh/MeshModel.h"
#include "graphos/core/project.h"

class QSettings;


namespace graphos
{

class MeshModelImp
  : public MeshModel
{

    Q_OBJECT

public:

    MeshModelImp(Project *project, QObject *parent = nullptr);
    ~MeshModelImp() override;

// MeshModel interface

public:

    auto properties() const -> std::shared_ptr<PoissonReconProperties> override;
    auto denseModel() const -> tl::Path override;
    auto projectDir() const -> tl::Path override;
    void setMesh(const tl::Path &mesh) override;

public slots:

    void setProperties(const std::shared_ptr<PoissonReconProperties> &properties) override;
    void setMeshReport(const MeshReport &report) override;

    void loadSettings() override;
    void saveSettings() override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;
    QSettings *mSettings;
    bool mReadSettings;
};

} // namespace graphos

#endif // GRAPHOS_MESH_MODEL_H
