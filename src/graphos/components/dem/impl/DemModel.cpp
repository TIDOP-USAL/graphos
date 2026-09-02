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

#include "DemModel.h"

#include "graphos/core/project/Project.h"
//#include "graphos/core/sfm/posesio.h"

//#include <tidop/geospatial/crs.h>
//#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>
#include <tidop/core/base/Split.h>
#include <tidop/core/base/TypeConversions.h>

#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include "DemTask.h"


namespace graphos
{

DemModelImp::DemModelImp(Project *project, QObject *parent)
  : DemModel(parent),
    mProject(project)
{
    DemModelImp::init();
}

auto DemModelImp::config() const -> std::shared_ptr<DemProperties>
{
    return mProject->demConfig();
}

auto DemModelImp::projectPath() const -> tl::Path
{
    return mProject->info().projectFolder();
}

auto DemModelImp::denseModel() const -> tl::Path
{
    return mProject->denseModel();
}

auto DemModelImp::crs() const -> std::string
{
    std::string epsg_code;

    try {

        if (auto config = mProject->demConfig()) {
            epsg_code = config->crs();
        }

        if (epsg_code.empty()) {
            auto enu_crs = enuCrs();
            auto v = tl::split<std::string>(enu_crs, ';');
            auto zone = tl::utmZoneFromLonLat(tl::convertStringTo<double>(v.at(1)), tl::convertStringTo<double>(v.at(2)));
            epsg_code = "EPSG:326";
            epsg_code.append(std::to_string(zone.first));
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return epsg_code;
}

//auto DemModelImp::gsd() const -> double
//{
//    return mProject->dem().gsd;
//}
//
//void DemModelImp::setGsd(double gsd)
//{
//    mProject->dem().gsd = gsd;
//}

//auto DemModelImp::dtmPath() const -> tl::Path
//{
//    return mProject->dtm();
//}
//
//auto DemModelImp::dsmPath() const -> tl::Path
//{
//    return mProject->dsm();
//}

void DemModelImp::cleanProject()
{
    mProject->clearDem();
}

void DemModelImp::setConfig(std::shared_ptr<DemProperties> config)
{
    mProject->setDemConfig(std::move(config));
}

void DemModelImp::setDtmPath(tl::Path dtmPath)
{
    mProject->setDtm(std::move(dtmPath));
}

void DemModelImp::setDsmPath(tl::Path dsmPath)
{
    mProject->setDsm(std::move(dsmPath));
}

void DemModelImp::setDsmEnuPath(tl::Path dsmPath)
{
    mProject->setDsmEnu(std::move(dsmPath));
}

//void DemModelImp::setCrs(const QString &crs)
//{
//    mProject->dem().epsgCode = crs;
//}

void DemModelImp::setReport(DemReport report)
{
    mProject->setDemReport(std::move(report));
}

auto DemModelImp::enuCrs() const -> std::string
{
    return mProject->info().enuCrs();
}

void DemModelImp::init()
{
}

void DemModelImp::clear()
{
}

} // namespace graphos


