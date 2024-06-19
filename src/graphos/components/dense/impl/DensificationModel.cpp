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

#include "graphos/core/project.h"
#include "graphos/core/dense/dense.h"
#include "graphos/core/sfm/groundpoint.h"

#include <tidop/core/console.h>

#include <QSettings>

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

auto DensificationModelImp::densification() const -> std::shared_ptr<Densification>
{
    ///TODO: Densificación guardada en proyecto y si no de configuración
    return mProject->densification();
}

auto DensificationModelImp::projectFolder() const -> tl::Path
{
    return mProject->projectFolder();
}


auto DensificationModelImp::reconstructionPath() const -> tl::Path
{
    return mProject->reconstructionPath();
}

auto DensificationModelImp::database() const -> tl::Path
{
    return mProject->database();
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

auto DensificationModelImp::images() const -> const std::unordered_map<size_t, Image>&
{
    return mProject->images();
}

auto DensificationModelImp::cameras() const -> const std::map<int, Camera>&
{
    return mProject->cameras();
}

auto DensificationModelImp::poses() const -> const std::unordered_map<size_t, CameraPose>&
{
    return mProject->poses();
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

void DensificationModelImp::setDensification(const std::shared_ptr<Densification> &densification)
{
    mProject->setDensification(densification);
}

void DensificationModelImp::setDenseModel(const tl::Path &denseModel)
{
    mProject->setDenseModel(denseModel);
}

void DensificationModelImp::setDenseReport(const DenseReport &report)
{
    mProject->setDenseReport(report);
}


} // End namespace graphos
