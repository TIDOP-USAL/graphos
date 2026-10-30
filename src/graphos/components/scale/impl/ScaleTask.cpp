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

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>

#include <cmath>

namespace graphos
{

ScaleTask::ScaleTask(double scale)
  : tl::Task(),
    mScale(scale),
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

auto ScaleTask::transform() const -> tl::Matrix<double, 4, 4>
{
    return mTransform;
}

void ScaleTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    tl::unusedParameter(progressBar);

    if (stopToken.stop_requested())
        return;
    TL_ASSERT(std::isfinite(mScale) && mScale > 0., "Scale factor must be positive and finite");
    mTransform[0][0] = mScale;
    mTransform[1][1] = mScale;
    mTransform[2][2] = mScale;
}

} // namespace graphos
