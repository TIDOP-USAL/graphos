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

#include "MeshCommand.h"

#include <tidop/core/app/Logger.h>
#include <tidop/core/console/ValuesValidator.h>

#include "graphos/core/mesh/PoissonReconProperties.h"
#include "graphos/core/project/Project.h"
#include "graphos/core/project/io/ProjectReader.h"
#include "graphos/core/project/io/ProjectWriter.h"
#include "graphos/components/mesh/impl/PoissonReconTask.h"

using namespace tl;

namespace graphos
{

MeshCommand::MeshCommand()
  : Command("mesh", "Poisson reconstruction mesh")
{
    PoissonReconProperties properties;

    this->addArgument<tl::Path>("prj", 'p', "Project file");
    this->addArgument<int>("depth", "Maximum reconstruction depth", properties.depth());
    //this->addArgument<int>("solve_depth", "Maximum solution depth", properties.solveDepth());
    auto arg_boundary_type = tl::Argument::make<std::string>("boundary_type", "Boundary type", properties.boundaryTypeAsText());
    std::vector<std::string> boundary_types{"Free", "Dirichlet", "Neumann"};
    arg_boundary_type->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(boundary_types));
    this->addArgument(arg_boundary_type);
    
    this->addExample("mesh -p 253/253.xml --depth 12 --boundary_type Dirichlet");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

MeshCommand::~MeshCommand() = default;

bool MeshCommand::run()
{

    bool r = false;

    auto &log = tl::Logger::instance();

    try {

        tl::Path project_path = this->value<tl::Path>("prj");
        int depth = this->value<int>("depth");
        //int solve_depth = this->value<int>("solve_depth");
        std::string boundary_type = this->value<std::string>("boundary_type");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        Project project;
        ProjectReader reader;
        reader.read(project_path, project);

        tl::Path point_cloud_path = project.denseModel();
        tl::Path mesh_path = project.info().projectFolder();
        mesh_path.append("dense").append("mesh.pr.ply");


        PoissonReconProperties::BoundaryType bt;
        if (boundary_type == "Free") {
            bt = PoissonReconProperties::BoundaryType::free;
        } else if (boundary_type == "Dirichlet") {
            bt = PoissonReconProperties::BoundaryType::dirichlet;
        } else {
            bt = PoissonReconProperties::BoundaryType::neumann;
        }

        auto properties = std::make_shared<PoissonReconProperties>();
        properties->setBoundaryType(bt);
        properties->setDepth(depth);

        auto task = std::make_shared<PoissonReconTask>(point_cloud_path, mesh_path, properties);
        task->run();

        project.setMeshConfig(properties);
        project.setMeshModel(mesh_path);
        project.setMeshReport(task->report());

        ProjectWriter writer;
        writer.write(project_path, project);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

}