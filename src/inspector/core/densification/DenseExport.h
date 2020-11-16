#ifndef INSPECTOR_DENSE_EXPORT_H
#define INSPECTOR_DENSE_EXPORT_H

#include <tidop/core/flags.h>

#include <QString>

namespace inspector
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

  DenseExport(const QString &denseModel);

  void setOffset(double x, double y, double z);
  void exportToCSV(const QString &csv,
                   const tl::EnumFlags<Fields> &flag);

protected:

  QString mDenseModel;
  double mOffsetX;
  double mOffsetY;
  double mOffsetZ;
};

} // namespace inspector

#endif // INSPECTOR_DENSE_EXPORT_H
