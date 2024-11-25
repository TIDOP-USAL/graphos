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

#include "ExportPointCloudModel.h"

#include <QFile>

#include "graphos/core/project.h"
#include "graphos/core/Application.h"

#include <QStandardPaths>
#include <tidop/geospatial/crs.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>

#include "graphos/core/ply.h"
#include "graphos/core/sfm/posesio.h"

namespace tl
{
    class EcefToEnu;
}

namespace graphos
{

ExportPointCloudModelImp::ExportPointCloudModelImp(Project *project,
                                                   QObject *parent)
  : ExportPointCloudModel(parent),
    mProject(project)
{
    ExportPointCloudModelImp::init();
}

ExportPointCloudModelImp::~ExportPointCloudModelImp()
{
}

auto ExportPointCloudModelImp::graphosProjectsDirectory() const -> tl::Path
{
    return Application::documentsLocation();
}

auto ExportPointCloudModelImp::pointCloud() const -> tl::Path
{
    return mProject->denseModel();
}

auto ExportPointCloudModelImp::enuCrs() const -> QString
{
    return mProject->enuCrs();
}

auto ExportPointCloudModelImp::crs() const -> QString
{
    QString epsg_code;

    try {

        auto enu_crs = mProject->enuCrs();

        auto v = tl::split<std::string>(enu_crs.toStdString(), ';');

        auto zone = tl::utmZoneFromLonLat(tl::stringToNumber<double>(v.at(1)), tl::stringToNumber<double>(v.at(2)));
        epsg_code = "EPSG:326";
        epsg_code.append(QString::number(zone.first));

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return epsg_code;
}

auto ExportPointCloudModelImp::pointCloudSize() const -> size_t
{
    return mProject->denseReport().points;
}

void ExportPointCloudModelImp::init()
{
}

void ExportPointCloudModelImp::clear()
{
}

} // namespace graphos
