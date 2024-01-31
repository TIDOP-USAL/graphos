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

#include "graphos/core/mesh/PoissonRecon.h"
#include "graphos/core/project.h"

#include <tidop/core/chrono.h>

using namespace tl;

namespace graphos
{

MeshCommand::MeshCommand()
  : Command("mesh", "Poisson reconstruction mesh")
{
    PoissonReconProperties properties;

    this->addArgument<std::string>("prj", 'p', "Project file");
    this->addArgument<int>("depth", "Maximum reconstruction depth", properties.depth());
    this->addArgument<int>("solve_depth", "Maximum solution depth", properties.solveDepth());
    auto arg_boundary_type = tl::Argument::make<std::string>("boundary_type", "Boundary type", properties.boundaryType().toStdString());
    std::vector<std::string> boundary_types{"free", "Dirichlet", "Neumann"};
    arg_boundary_type->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(boundary_types));
    this->addArgument(arg_boundary_type);
    this->addArgument<int>("grid_width", "Grid width", properties.width());
    this->addArgument<int>("full_depth", "Full depth", properties.fullDepth());
    
    this->addExample("mesh -p 253/253.xml --method PMVS");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

MeshCommand::~MeshCommand()
{
}

bool MeshCommand::run()
{

    bool r = false;

    QString file_path;
    QString project_path;

    try {

        //tl::Chrono chrono("Poisson Reconstruction finished");
        //chrono.run();

        tl::Path prj_path = this->value<std::string>("prj");
        int depth = this->value<int>("depth");
        int solve_depth = this->value<int>("solve_depth");
        std::string boundary_type = this->value<std::string>("boundary_type");
        int grid_width = this->value<int>("grid_width");
        int full_depth = this->value<int>("full_depth");

        TL_ASSERT(prj_path.exists(), "Project doesn't exist");
        TL_ASSERT(prj_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(prj_path);

        tl::Path point_cloud_path = project.denseModel();
        tl::Path mesh_path = project.projectFolder();
        mesh_path.append("dense").append("mesh.pr.ply");

        auto process = std::make_shared<PoissonReconTask>(point_cloud_path,
                                                          mesh_path);
        process->setBoundaryType(QString::fromStdString(boundary_type));
        process->setDepth(depth);
        process->setFullDepth(full_depth);
        process->setSolveDepth(solve_depth);
        process->setWidth(grid_width);

        process->run();

        project.setProperties(process);
        project.setMeshPath(mesh_path);
        project.save(prj_path);

        //chrono.stop();

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    return false;
}

}