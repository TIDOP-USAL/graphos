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

#include "DensificationModel.h"

#include "graphos/core/project/Project.h"
//#include "graphos/core/dense/dense.h"
//#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/orientation/GroundPoint.h"

#include <tidop/core/console/Console.h>

#include <QSettings>
#include <QCoreApplication>

namespace graphos
{


DensificationModelImp::DensificationModelImp(Project *project,
                                             QObject *parent)
  : DensificationModel(parent),
    mProject(project)
{
    DensificationModelImp::init();
}

DensificationModelImp::~DensificationModelImp()
{

}

void DensificationModelImp::init()
{
}

void DensificationModelImp::clear()
{
    mProject->clearDensification();
}

auto DensificationModelImp::densificationConfig() const -> std::shared_ptr<DensificationProperties>
{
    return mProject->densificationConfig();
}

auto DensificationModelImp::projectFolder() const -> tl::Path
{
    return mProject->info().projectFolder();
}

//auto DensificationModelImp::reconstructionPath() const -> tl::Path
//{
//    return mProject->reconstructionPath();
//}

auto DensificationModelImp::database() const -> tl::Path
{
    return mProject->info().database();
}

auto DensificationModelImp::useCuda() const -> bool
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
    return settings.value("UseCuda", true).toBool();
}

auto DensificationModelImp::existDenseModel() const -> bool
{
    tl::Path dense_model = mProject->denseModel();
    return !dense_model.empty();
}

auto DensificationModelImp::images() const -> const ImageRepository &
{
    return mProject->images();
}

auto DensificationModelImp::cameras() const -> const CameraRepository &
{
    return mProject->cameras();
}

auto DensificationModelImp::cameraPoses() const -> const CameraPosesRepository &
{
    return mProject->cameraPoses();
}

auto DensificationModelImp::groundPoints() const -> std::vector<GroundPoint>
{
    std::vector<GroundPoint> ground_points;

    try {

        std::unique_ptr<GroundPointsReader> reader = GroundPointsReaderFactory::create("GRAPHOS");
        reader->read(mProject->groundPoints());
        ground_points = reader->points();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return ground_points;
}

void DensificationModelImp::cleanProject()
{
    mProject->clearDensification();
}

void DensificationModelImp::setDensificationConfig(std::shared_ptr<DensificationProperties> config)
{
    mProject->setDensificationConfig(std::move(config));
}

void DensificationModelImp::setDenseModel(tl::Path denseModel)
{
    mProject->setDenseModel(std::move(denseModel));
}

void DensificationModelImp::setDensificationReport(DensificationReport report)
{
    mProject->setDensificationReport(std::move(report));
}


} // End namespace graphos
