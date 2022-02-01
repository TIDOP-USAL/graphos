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

#ifndef GRAPHOS_COLMAP_CAMERA_DATABASE_H
#define GRAPHOS_COLMAP_CAMERA_DATABASE_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>

#include <tidop/geospatial/camera.h>

class QSqlDatabase;

namespace colmap
{
class Reconstruction;
}

namespace graphos
{

class GRAPHOS_EXPORT DatabaseCameras
{

public:

  DatabaseCameras(QString database);
  ~DatabaseCameras();

  void open();
  bool isOpen() const;
  void close();

  bool existCameraMakeId(const QString &cameraMake) const;
  bool existCameraModel(int cameraMake,
                        const QString &cameraModel) const;

  int cameraMakeId(const QString &cameraMake) const;
  double cameraSensorSize(int cameraMake,
                          const QString &cameraModel) const;

private:

  QSqlDatabase *mDatabase;
};


} // namespace graphos

#endif // GRAPHOS_COLMAP_CAMERA_DATABASE_H
