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

auto ExportPointCloudModelImp::offset() const -> tl::Point3<double>
{
    return offsetRead(mProject->offset());
}

auto ExportPointCloudModelImp::crs() const -> QString
{
    QString epsg_code = mProject->dem().epsgCode;

    if (epsg_code.isEmpty()) {
        auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
        auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");
        tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
        auto lla = crs_transfom_geocentric_to_geographic.transform(offset());
        int zone = tl::utmZoneFromLongitude(lla.x);
        epsg_code = "EPSG:326";
        epsg_code.append(QString::number(zone));
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
