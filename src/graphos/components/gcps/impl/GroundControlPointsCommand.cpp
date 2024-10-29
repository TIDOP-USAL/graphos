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


#include "GroundControlPointsCommand.h"

#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/posesio.h"
#include "graphos/core/project.h"

#include <tidop/core/msg/message.h>
#include <tidop/math/geometry/affine.h>

#include <QFileInfo>
#include <tidop/core/log.h>


namespace graphos
{


GroundControlPointsCommand::GroundControlPointsCommand()
  : Command("gcps", "Import Ground Control Points")
{
    this->addArgument<tl::Path>("prj", 'p', "Project file");
    this->addArgument<tl::Path>("cp", "Ground Control Points file");
    this->addArgument<std::string>("crs", "CRS", "");

    this->addExample("georef --p 253/253.xml --cp georef.xml");
}

GroundControlPointsCommand::~GroundControlPointsCommand() = default;

auto GroundControlPointsCommand::readGroundPoints(const tl::Path &groundPointsPath) -> std::vector<GroundPoint>
{
    auto reader = GroundPointsReaderFactory::create("GRAPHOS");
    reader->read(groundPointsPath);

    return reader->points();
}

bool GroundControlPointsCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<tl::Path>("prj");
        tl::Path gcp = this->value<tl::Path>("cp");
        std::string crs = this->value<std::string>("crs");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");
        TL_ASSERT(gcp.isFile(), "GCP file doesn't exist");

        ProjectImp project;
        project.load(project_path);

        std::string format;
        if (tl::compareInsensitiveCase(gcp.extension().toString(), ".xml")) {
            format = "GRAPHOS";
        } else if (tl::compareInsensitiveCase(gcp.extension().toString(), ".txt")) {
            format = "ODM";
        } else {
            throw std::runtime_error("Ground control point format invalid");
        }

        auto reader = GCPsReaderFactory::create(format);
        reader->setImages(project.images());
        reader->read(gcp);
        std::vector<GroundControlPoint> ground_control_points = reader->gcps();

        if (crs.empty()) {
            crs = reader->epsgCode();
        }

        tl::Path gcp_file = project.projectFolder();
        gcp_file.append("sfm");
        gcp_file.createDirectories();
        gcp_file.append("georef.xml");

        auto writer = GCPsWriterFactory::create("GRAPHOS");
        writer->setEPSGCode(crs);
        writer->setGCPs(reader->gcps());
        writer->setImages(project.images());
        writer->write(gcp_file);

        project.setCrs(QString::fromStdString(crs));
        project.save(project_path);

    } catch (const std::exception &e) {

        tl::printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos