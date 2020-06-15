#ifndef INSPECTOR_CORE_PHOTO_ORIENTATION_H
#define INSPECTOR_CORE_PHOTO_ORIENTATION_H

#include <array>

#include <QString>

#include "inspector/inspector_global.h"

namespace colmap
{
class Reconstruction;
}

namespace inspector
{

struct INSPECTOR_EXPORT PhotoOrientation
{
  double x = 0.;
  double y = 0.;
  double z = 0.;
  std::array<std::array<float, 3>, 3> rot;
};


class INSPECTOR_EXPORT ReadPhotoOrientations
{

public:

  ReadPhotoOrientations();
  ~ReadPhotoOrientations();

  void open(const QString &path);
  PhotoOrientation orientation(const QString &image) const;

protected:

  colmap::Reconstruction *mReconstruction;

};

} // namespace inspector

#endif // INSPECTOR_CORE_PHOTO_ORIENTATION_H
