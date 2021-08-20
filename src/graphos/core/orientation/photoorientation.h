#ifndef GRAPHOS_CORE_PHOTO_ORIENTATION_H
#define GRAPHOS_CORE_PHOTO_ORIENTATION_H

#include <array>

#include <QString>

#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/rotation_matrix.h>

#include "graphos/graphos_global.h"

namespace colmap
{
class Reconstruction;
}

namespace graphos
{

struct GRAPHOS_EXPORT CameraPose
{
  tl::Point3D position{};
  tl::math::RotationMatrix<double> rotation{};
};


class GRAPHOS_EXPORT ReadPhotoOrientations
{

public:

  ReadPhotoOrientations();
  ~ReadPhotoOrientations();

  void open(const QString &path);
  CameraPose orientation(const QString &image) const;

protected:

  colmap::Reconstruction *mReconstruction;

};

} // namespace graphos

#endif // GRAPHOS_CORE_PHOTO_ORIENTATION_H
