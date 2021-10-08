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
