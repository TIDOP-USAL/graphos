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

#ifndef GRAPHOS_LOAD_IMAGES_PROCESS_H
#define GRAPHOS_LOAD_IMAGES_PROCESS_H

#include "graphos/process/ProcessConcurrent.h"
#include "graphos/core/image.h"

namespace tl
{
class ImageReader;

namespace geospatial
{
class Crs;
}

}

namespace graphos
{

//class DatabaseCameras;
class Camera;

class GRAPHOS_EXPORT LoadImagesProcess
  : public ProcessConcurrent
{

  Q_OBJECT

public:

  LoadImagesProcess(std::vector<Image> *images, 
                    std::vector<Camera> *cameras,
                    const QString &epsg = QString());
  ~LoadImagesProcess() override;

signals:

  void imageAdded(int, int);

private:

  bool existCamera(const QString &make, const QString &model) const;
  int findCamera(const QString &make, const QString &model) const;
  void loadImage(size_t imageId);
  int loadCamera(tl::ImageReader *imageReader);

// ProcessConcurrent interface

protected:

  void run() override;

  double parseFocal(const std::string &focal, double def);

protected:

  std::vector<Image> *mImages;
  std::vector<Camera> *mCameras;
  QString mEPSG;
  std::shared_ptr<tl::geospatial::Crs> mCrsIn;
  std::shared_ptr<tl::geospatial::Crs> mCrsOut;
  //DatabaseCameras *mDatabaseCameras;
  QString mDatabaseCamerasPath;
};

} // graphos

#endif // LOADIMAGESPROCESS_H
