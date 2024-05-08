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

#include "MeshModel.h"

#include "graphos/core/mesh/PoissonRecon.h"

#include <QFile>
#include <QSettings>

namespace graphos
{

MeshModelImp::MeshModelImp(Project *project, QObject *parent)
  : MeshModel(parent),
    mProject(project),
    mSettings(new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName()))
{
    this->MeshModelImp::init();
}

MeshModelImp::~MeshModelImp()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

void MeshModelImp::loadSettings()
{
}

void MeshModelImp::saveSettings()
{
}

auto MeshModelImp::properties() const -> std::shared_ptr<PoissonReconProperties>
{
    return mProject->meshProperties();
}

auto MeshModelImp::denseModel() const -> tl::Path
{
    return mProject->denseModel();
}

auto MeshModelImp::projectDir() const -> tl::Path
{
    return mProject->projectFolder();
}

auto MeshModelImp::mesh() const -> tl::Path
{
    return mProject->meshPath();
}

void MeshModelImp::setMesh(const tl::Path &mesh)
{
    mProject->setMeshPath(mesh);
}

void MeshModelImp::cleanProject()
{
    mProject->setMeshPath(tl::Path());
    mProject->setMeshReport(MeshReport());
}

void MeshModelImp::setProperties(const std::shared_ptr<PoissonReconProperties> &properties)
{
    mProject->setMeshProperties(properties);
}

void MeshModelImp::setMeshReport(const MeshReport &report)
{
    mProject->setMeshReport(report);
}

void MeshModelImp::init()
{
    mReadSettings = mSettings->value("GENERAL/SAVE_PARAMETERS", false).toBool();
}

void MeshModelImp::clear()
{

}

} // namespace graphos
