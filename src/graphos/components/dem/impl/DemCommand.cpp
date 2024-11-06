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


#include "DemCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/components/dem/impl/DemTask.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/log.h>

#include <QFileInfo>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>


using namespace tl;

namespace graphos
{


DemCommand::DemCommand()
  : Command("dem", "Create DSM and/or DTM"),
    mProject(nullptr)
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<double>("gsd", 'g', "Ground sample distance", 0.1);
    this->addArgument<bool>("dsm", "Create a Digital Surface Model", true);
    this->addArgument<bool>("dtm", "Create a Digital Terrain Model", false);
    this->addArgument<std::string>("crs", "Coordinate Reference System", "");

    this->addExample("dem -p 253/253.xml --gsd 0.1");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

DemCommand::~DemCommand()
{
    if (mProject) {
        delete mProject;
        mProject = nullptr;
    }
}

bool DemCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<Path>("prj");
        auto gsd =  this->value<double>("gsd");
        auto dsm =  this->value<bool>("dsm");
        auto dtm =  this->value<bool>("dtm");
        auto crs =  this->value<std::string>("crs");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        mProject = new ProjectImp;
        mProject->load(project_path);

        tl::Path dem_path(mProject->projectFolder());
        dem_path.append("dem");
		
        tl::Path ground_points_path(mProject->reconstructionPath());
        ground_points_path.append("ground_points.bin");

        tl::Point3<double> offset = offsetRead(mProject->offset());

        if (crs.empty()){

            // Esto no tiene que hacerse ya que vamos a tener las coordenadas geograficas directamente
            auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
            auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");
            tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
            auto lla = crs_transfom_geocentric_to_geographic.transform(offset);

            //auto zone = tl::utmZoneFromLonLat(lla.x, lla.y);
            int zone = tl::utmZoneFromLongitude(lla.x);
            crs = "EPSG:326";
            crs.append(std::to_string(zone));
        }

        DemTask dem_task(mProject->denseModel(), offset, dem_path, gsd, crs, dsm, dtm);
        dem_task.run();

        tl::Path dsm_file = dem_path;
        dsm_file.append("dsm.tif");
        if (dsm && dsm_file.exists()) {
            mProject->dem().dsmPath = dsm_file;
        }
        
        tl::Path dtm_file = dsm_file;
        dtm_file.replaceBaseName("dem");
        if (dtm && dtm_file.exists()) {
            mProject->dem().dtmPath = dtm_file;
        }

        mProject->dem().gsd = gsd;

        mProject->setDemReport(dem_task.report());

        mProject->save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos