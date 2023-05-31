/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include "ScaleTask.h"

#include <tidop/core/messages.h>
#include <tidop/core/exception.h>

#include <CCGeom.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>


namespace graphos
{

ScaleTask::ScaleTask(double scale, ccHObject *model/*, const QVector3D &offset*/)
  : tl::TaskBase(),
    mScale(scale),
    mModel(model)/*,
    mOffset(offset)*/
{

}

ScaleTask::~ScaleTask()
{

}

void ScaleTask::setScale(double scale)
{
  mScale = scale;
}

void ScaleTask::execute(tl::Progress *progressBar)
{

  try {
    
    bool lockedVertices;
	  //try to get the underlying cloud (or the vertices set for a mesh)
    ccGenericPointCloud* cloud = ccHObjectCaster::ToGenericPointCloud(mModel, &lockedVertices);

    // Se mantiene la nube de puntos centrada
    CCVector3 C(0, 0, 0);
    if (1/*keepInPlace*/)
      C = mModel->getOwnBB().getCenter();

    //ccHObjectContext objContext = removeObjectTemporarilyFromDBTree(mModel, cloud);

    cloud->scale(static_cast<PointCoordinateType>(mScale),
                 static_cast<PointCoordinateType>(mScale),
                 static_cast<PointCoordinateType>(mScale),
                 C);
    
    //putObjectBackIntoDBTree(cloud, objContext);
	  cloud->prepareDisplayForRefresh_recursive();
    mModel->prepareDisplayForRefresh_recursive();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("error");
  }

}

} // namespace graphos
