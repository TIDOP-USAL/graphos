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

#ifndef GRAPHOS_MESH_MODEL_INTERFACE_H
#define GRAPHOS_MESH_MODEL_INTERFACE_H

#include <array>

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"
#include "graphos/core/mesh/PoissonRecon.h"

namespace graphos
{

class PoissonReconProperties;

class MeshModel
  : public Model
{

    Q_OBJECT

public:

    MeshModel(QObject *parent = nullptr) : Model(parent) {}
    ~MeshModel() override = default;

    virtual std::shared_ptr<PoissonReconProperties> properties() const = 0;

    virtual tl::Path denseModel() const = 0;
    virtual tl::Path projectDir() const = 0;
    virtual void setMesh(const tl::Path &mesh) = 0;

public slots:

    virtual void setProperties(const std::shared_ptr<PoissonReconProperties> &properties) = 0;
    virtual void setMeshReport(const MeshReport &report) = 0;

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
};

} // namespace graphos


#endif // GRAPHOS_MESH_MODEL_INTERFACE_H
