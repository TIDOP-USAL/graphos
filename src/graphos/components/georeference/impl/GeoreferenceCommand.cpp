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


#include "GeoreferenceCommand.h"

#include "graphos/components/georeference/impl/GeoreferenceProcess.h"
#include "graphos/core/sfm/groundpoint.h"
#include "graphos/core/sfm/posesio.h"

#include <tidop/core/messages.h>

#include <QFileInfo>

namespace graphos
{
  

GeoreferenceCommand::GeoreferenceCommand()
  : Command("georef", "Georeference"),
    mProjectFile(""),
    mGCP("")
{
  this->push_back(CreateArgumentPathRequired("prj", 'p', "Project file", &mProjectFile));
  this->push_back(CreateArgumentPathOptional("cp", "Ground Control Points", &mGCP));

  this->addExample("georef --p 253/253.xml --cp 253/georef.xml");
}

GeoreferenceCommand::~GeoreferenceCommand()
{
}

bool GeoreferenceCommand::run()
{
  bool r = false;

  QString file_path;
  QString project_path;

  try {

    TL_ASSERT(mProjectFile.exists(), "Project doesn't exist");
    TL_ASSERT(mProjectFile.isFile(), "Project file doesn't exist");
    TL_ASSERT(mGCP.isFile(), "GCP file doesn't exist");

    QString project_file = QString::fromStdWString(mProjectFile.toWString());

    ProjectImp project;
    project.load(project_file);

    QString ori_relative = project.projectFolder() + "/ori/relative/";
    QString ori_absolute = project.projectFolder() + "/ori/absolute/";

    auto reader = GCPsReaderFactory::create("GRAPHOS");
    reader->read(mGCP);
    std::vector<GroundControlPoint> ground_control_points = reader->gcps();
    //std::vector<GroundControlPoint> ground_control_points = groundControlPointsRead(mGCP);

    //GeoreferenceProcess georeference_process(ori_relative,
    //                                         ori_absolute,
    //                                         ground_control_points);

    //georeference_process.run();

    //QString sparse_model = ori_absolute + "/sparse.ply";

    //if(QFileInfo::exists(sparse_model)) {

    //  project.setReconstructionPath(ori_absolute);
    //  project.setSparseModel(sparse_model);
    //  project.setOffset(ori_absolute + "/offset.txt");

    //  ReadCameraPoses readPhotoOrientations;
    //  readPhotoOrientations.open(ori_absolute);

    //  for (const auto &image : project.images()){
    //    size_t image_id = image.first;
    //  //for(auto image = project.imageBegin(); image != project.imageEnd(); image++) {
    //    CameraPose photoOrientation = readPhotoOrientations.orientation(image.second.name());
    //    if(photoOrientation.position() != tl::Point3D()) {
    //      project.addPhotoOrientation(image_id, photoOrientation);
    //    }
    //  }

    //}

    project.save(project_file);

  } catch (const std::exception& e) {

    tl::printException(e);

    r = true;
  }

  return r;
}

} // namespace graphos