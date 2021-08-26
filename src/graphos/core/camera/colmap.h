#ifndef GRAPHOS_COLMAP_CAMERA_H
#define GRAPHOS_COLMAP_CAMERA_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>

#include <tidop/geospatial/camera.h>

namespace colmap
{
class Reconstruction;
}

namespace graphos
{

class GRAPHOS_EXPORT ReadCalibration
{

public:

  ReadCalibration();
  ~ReadCalibration();

  void open(const QString &path);
  std::shared_ptr<tl::Calibration> calibration(int cameraId) const;

protected:

  colmap::Reconstruction *mReconstruction;

};

QString cameraToColmapType(const tl::Camera &camera);

} // namespace graphos

#endif // GRAPHOS_COLMAP_CAMERA_H
