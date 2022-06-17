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
  : Command("mesh", "Poisson reconstruction mesh"),
    mProjectFile("")
{
  initDefaultParameters();

  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentIntegerOptional("depth", std::string("Maximum reconstruction depth (default=").append(std::to_string(mDepth)).append(")"), &mDepth));
  this->push_back(CreateArgumentIntegerOptional("solve_depth", std::string("Maximum solution depth (default=").append(std::to_string(mSolveDepth)).append(")"), &mSolveDepth));
  this->push_back(CreateArgumentListStringOptional("boundary_type", std::string("Boundary type (default=").append(mBoundaryTypes[mBoundaryTypeIndex]).append(")"), mBoundaryTypes, &mBoundaryTypeIndex));
  this->push_back(CreateArgumentIntegerOptional("grid_width", std::string("Grid width (default=").append(std::to_string(mWidth)).append(")"), &mWidth));
  this->push_back(CreateArgumentIntegerOptional("full_depth", std::string("Full depth (default=").append(std::to_string(mFullDepth)).append(")"), &mFullDepth));

  this->addExample("mesh --p 253/253.xml --method PMVS");

  this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

MeshCommand::~MeshCommand()
{
}

void MeshCommand::initDefaultParameters()
{
  PoissonReconParameters parameters;
  mDepth = parameters.depth();
  mSolveDepth = parameters.solveDepth();
  mBoundaryTypes = {"free", "Dirichlet", "Neumann"};
  std::string default_boudary_type = parameters.boundaryType().toStdString();
  for(size_t i = 0; i < mBoundaryTypes.size(); i++) {
    if(mBoundaryTypes[i] == default_boudary_type) {
      mBoundaryTypeIndex = i;
    }
  }
  mWidth = parameters.width();
  mFullDepth = parameters.fullDepth();
}

bool MeshCommand::run()
{

  bool r = false;

  QString file_path;
  QString project_path;

  try {

    tl::Chrono chrono("Poisson Reconstruction finished");
    chrono.run();

    TL_ASSERT(mProjectFile.exists(), "Project doesn't exist");
    TL_ASSERT(mProjectFile.isFile(), "Project file doesn't exist");

    QString project_file = QString::fromStdWString(mProjectFile.toWString());

    ProjectImp project;
    project.load(project_file);

    QString point_cloud = project.denseModel();
    QString mesh = project.projectFolder();
    mesh.append("\\dense\\mesh.pr.ply");

    auto process = std::make_shared<PoissonReconTask>(point_cloud,
                                                      mesh);
    process->setBoundaryType(QString::fromStdString(mBoundaryTypes[mBoundaryTypeIndex]));
    process->setDepth(mDepth);
    process->setFullDepth(mFullDepth);
    process->setSolveDepth(mSolveDepth);
    process->setWidth(mWidth);

    project.setMeshParameters(process);
    project.setMeshPath(mesh);
    project.save(project_file);
    
    chrono.stop();
    
  } catch(const std::exception &e) {
    
    printException(e);
    
    r = true;
  }
    return false;
}

}