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

#include <tidop/core/msg/message.h>
#include <tidop/core/exception.h>

#include <CCGeom.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <ccHObjectCaster.h>

namespace graphos
{

ScaleTask::ScaleTask(double scale, ccHObject *model)
  : tl::TaskBase(),
    mScale(scale),
    mModel(model),
    mTransform(tl::Matrix<double, 4, 4>::identity())
{

}

ScaleTask::~ScaleTask()
{

}

void ScaleTask::setScale(double scale)
{
    mScale = scale;
}

tl::Matrix<double, 4, 4> ScaleTask::transform() const
{
    return mTransform;
}

void ScaleTask::execute(tl::Progress *progressBar)
{
    tl::unusedParameter(progressBar);

    try {

        bool lockedVertices;
        ccGenericPointCloud *cloud = ccHObjectCaster::ToGenericPointCloud(mModel, &lockedVertices);

        CCVector3 center(0, 0, 0);

        cloud->scale(static_cast<PointCoordinateType>(mScale),
                     static_cast<PointCoordinateType>(mScale),
                     static_cast<PointCoordinateType>(mScale),
                     center);

        mTransform[0][0] = mScale;
        mTransform[1][1] = mScale;
        mTransform[2][2] = mScale;
        mTransform[0][3] = -mScale * static_cast<double>(center.x) + static_cast<double>(center.x);
        mTransform[1][3] = -mScale * static_cast<double>(center.y) + static_cast<double>(center.y);
        mTransform[2][3] = -mScale * static_cast<double>(center.z) + static_cast<double>(center.z);

        cloud->prepareDisplayForRefresh_recursive();
        mModel->prepareDisplayForRefresh_recursive();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("error");
    }

}

} // namespace graphos
