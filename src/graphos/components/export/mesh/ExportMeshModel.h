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

#ifndef GRAPHOS_EXPORT_MESH_MODEL_INTERFACE_H
#define GRAPHOS_EXPORT_MESH_MODEL_INTERFACE_H

#include <tidop/core/path.h>

#include "graphos/core/mvp.h"

namespace graphos
{

/*!
 * \brief ExportMeshModel interface
 */
class ExportMeshModel
  : public Model
{

    Q_OBJECT

public:

    ExportMeshModel(QObject *parent = nullptr) : Model(parent) {}
    ~ExportMeshModel() override = default;

    virtual auto graphosProjectsDirectory() const -> tl::Path = 0;

public slots:

    virtual void exportMesh(const tl::Path &exportPath) = 0;

};

} // namespace graphos

#endif // GRAPHOS_EXPORT_MESH_MODEL_INTERFACE_H
