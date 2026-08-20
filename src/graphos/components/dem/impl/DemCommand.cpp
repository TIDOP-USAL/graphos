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
#include "graphos/core/project/Project.h"
#include "graphos/core/project/io/ProjectReader.h"
#include "graphos/core/project/io/ProjectWriter.h"
//#include "graphos/core/orientation/posesio.h"
#include "graphos/components/dem/impl/DemTask.h"
#include "graphos/core/task/Progress.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/app/Logger.h>
#include <tidop/core/base/Split.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>
#include <tidop/geotools/GeoTools.h>

#include <QFileInfo>


using namespace tl;

namespace graphos
{


DemCommand::DemCommand()
  : Command("dem", "Create DSM and/or DTM"),
    mGeoTools(tl::GeoTools::getInstance())
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<double>("gsd", 'g', "Ground sample distance", 0.1);
    this->addArgument<bool>("dsm", "Create a Digital Surface Model", true);
    this->addArgument<bool>("dtm", "Create a Digital Terrain Model", false);
    this->addArgument<std::string>("crs", "Coordinate Reference System", "");
    //auto arg_progress_bar = tl::Argument::make<std::string>("progress_bar", "Type of progress bar", "COLOR");
    //auto progress_bar_validator = tl::ValuesValidator<std::string>::create({"NORMAL", "COLOR", "PERCENT", "SPINNER", "DISABLE"});
    //arg_progress_bar->setValidator(progress_bar_validator);
    //this->addArgument(arg_progress_bar);

    this->addExample("dem -p 253/253.xml --gsd 0.1");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

DemCommand::~DemCommand()
{
}

auto DemCommand::crs(std::string_view enuCrs) const -> std::string
{
    std::string epsg_code;

    try {

        auto v = tl::split<std::string>(enuCrs, ';');
        auto zone = tl::utmZoneFromLonLat(tl::convertStringTo<double>(v.at(1)), tl::convertStringTo<double>(v.at(2)));
        epsg_code = "EPSG:326";
        epsg_code.append(std::to_string(zone.first));

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return epsg_code;
}

bool DemCommand::run()
{
    bool r = false;

    auto &log = tl::Logger::instance();

    try {

        auto project_path = this->value<Path>("prj");
        auto gsd =  this->value<double>("gsd");
        auto dsm =  this->value<bool>("dsm");
        auto dtm =  this->value<bool>("dtm");
        auto crs =  this->value<std::string>("crs");
        //auto progress_bar = this->value<std::string>("progress_bar");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        Project project;
        ProjectReader reader;
        reader.read(project_path, project);

        tl::Path dem_path(project.info().projectFolder());
        dem_path.append("dem");


        if (crs.empty()) crs = this->crs(project.info().enuCrs());

        auto dem_properties = std::make_shared<DemProperties>();
        dem_properties->setCrs(crs);
        dem_properties->setGsd(gsd);
        dem_properties->setDsm(dsm);
        dem_properties->setDtm(dtm);
           
        DemTask dem_task(project.denseModel(), dem_path, project.info().enuCrs(), dem_properties);
        dem_task.run(/*progress.get()*/);

        tl::Path dsm_file = dem_path;
        dsm_file.append("dsm.tif");
        if (dsm && dsm_file.exists()) {
            project.setDsm(dsm_file);
        }
        
        tl::Path dtm_file = dsm_file;
        dtm_file.replaceBaseName("dem");
        if (dtm && dtm_file.exists()) {
            project.setDtm(dtm_file);
        }

        project.setDemConfig(dem_properties);
        project.setDemReport(dem_task.report());

        ProjectWriter writer;
        writer.write(project_path, project);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos