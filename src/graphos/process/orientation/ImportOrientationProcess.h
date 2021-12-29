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

#ifndef GRAPHOS_IMPORT_ORIENTATION_PROCESS_H
#define GRAPHOS_IMPORT_ORIENTATION_PROCESS_H

#include <tidop/geospatial/camera.h>

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/image.h"

namespace graphos
{

class GRAPHOS_EXPORT ImportOrientationProcess
  : public ProcessConcurrent
{
  Q_OBJECT

public:

  ImportOrientationProcess(const std::vector<Image> &images, 
                           const std::map<int, tl::Camera> &cameras,
                           const QString &projectPath, 
                           const QString &database,
                           bool fixCalibration,
                           bool fixPoses);
  ~ImportOrientationProcess() override;

signals:

  void importOrientationFinished();

// ProcessConcurrent interface

protected:

  void run() override;

private:

  std::vector<Image> mImages;
  std::map<int, tl::Camera> mCameras;
  QString mProjectPath;
  QString mDatabase;
  bool mFixCalibration;
  bool mFixPoses;
};

} // namespace graphos

#endif // GRAPHOS_IMPORT_ORIENTATION_PROCESS_H
