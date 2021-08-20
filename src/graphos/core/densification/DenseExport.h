#ifndef GRAPHOS_DENSE_EXPORT_H
#define GRAPHOS_DENSE_EXPORT_H

#include <tidop/core/flags.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/geometry/entities/bbox.h>

namespace graphos
{

class DenseExport
{

public:

  enum class Fields
  {
    xyz = 1 << 0,
    rgb = 1 << 1,
    normals = 1 << 2
  };

public:

  DenseExport(const std::string &denseModel);

  void setOffset(const tl::Point3<double> &point);
  void exportToCSV(const std::string &csv,
                   const tl::EnumFlags<Fields> &flag,
                   tl::BoundingBox<tl::Point3<double>> *bbox = nullptr);

protected:

  std::string mDenseModel;
  tl::Point3<double> mOffset;
};

} // namespace graphos

#endif // GRAPHOS_DENSE_EXPORT_H
