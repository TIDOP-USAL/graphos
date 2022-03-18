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

#ifndef GRAPHOS_CORE_CAMERA_POSES_IO_H
#define GRAPHOS_CORE_CAMERA_POSES_IO_H

#include "graphos/graphos_global.h"

#include <QString>

#include "graphos/core/orientation/poses.h"

namespace colmap
{
class Reconstruction;
}

namespace graphos
{

class GRAPHOS_EXPORT ReadCameraPoses
{

public:

  ReadCameraPoses();
  ~ReadCameraPoses();

  void open(const QString &path);
  CameraPose orientation(const QString &image) const;

protected:

  colmap::Reconstruction *mReconstruction;

};

} // namespace graphos

#endif // GRAPHOS_CORE_CAMERA_POSES_IO_H
