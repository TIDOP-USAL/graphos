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

#ifndef GRAPHOS_GEOREFERENCE_PROCESS_H
#define GRAPHOS_GEOREFERENCE_PROCESS_H

#include "graphos/core/sfm/groundpoint.h"

#include "graphos/core/image.h"

#include <tidop/core/task.h>
#include <tidop/core/progress.h>
#include <tidop/core/path.h>

#include <QObject>

namespace graphos
{

class CameraPose;
class Camera;

class GRAPHOS_EXPORT GeoreferenceProcess
  : public QObject,
    public tl::TaskBase
{
  Q_OBJECT

public:

  GeoreferenceProcess(const std::unordered_map<size_t, Image> &images,
                      const std::map<int, Camera> &cameras,
                      const std::unordered_map<size_t, CameraPose> &poses,
                      const std::vector<GroundPoint> &groundPoints,
                      const std::vector<GroundControlPoint> &groundControlPoints,
                      const tl::Path &database);
  ~GeoreferenceProcess() override;

signals:

  void georeferenceFinished();

// tl::TaskBase interface

protected:

  void execute(tl::Progress *progressBar) override;

private:

  std::unordered_map<size_t, Image> mImages;
  std::map<int, Camera> mCameras;
  std::unordered_map<size_t, CameraPose> mPoses;
  std::vector<GroundPoint> mGroundPoints;
  std::vector<GroundControlPoint> mGroundControlPoints;
  tl::Path mDatabase;
};

} // namespace graphos

#endif // GRAPHOS_GEOREFERENCE_PROCESS_H
