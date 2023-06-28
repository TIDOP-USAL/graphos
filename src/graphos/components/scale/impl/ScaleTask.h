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

#ifndef GRAPHOS_SCALE_TASK_H
#define GRAPHOS_SCALE_TASK_H

#include <vector>
#include <map>

#include <QString>
#include <QVector3D>

#include <tidop/core/task.h>
#include <tidop/math/algebra/matrix.h>

class ccHObject;


namespace tl
{
class Task;
}

namespace graphos
{
	
class ScaleAlgorithm;

class ScaleTask
  : public tl::TaskBase
{

public:

  ScaleTask(double scale, ccHObject *model);
  ~ScaleTask();

public:

  void setScale(double);
  tl::math::Matrix<double, 4, 4> transform() const;

// tl::TaskBase interface

protected:

  void execute(tl::Progress *progressBar) override;
  
private:

  double mScale;
  ccHObject *mModel;
  tl::math::Matrix<double, 4, 4> mTransform;
};

} // namespace graphos

#endif // GRAPHOS_SCALE_TASK_H
