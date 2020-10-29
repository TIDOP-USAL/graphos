#ifndef INSPECTOR_DENSE_EXPORT_H
#define INSPECTOR_DENSE_EXPORT_H

#include <QString>

namespace inspector
{

class DenseExport
{
  enum class Fields
  {
    xyz = 0,
    rgb = 1 << 0,
    normals = 1 << 1
  };

public:

  DenseExport(const QString &denseModel);

  void setOffset(double x, double y, double z);
  void exportToCSV(const QString &csv, Fields fields);

protected:

  QString mDenseModel;
  double mOffsetX;
  double mOffsetY;
  double mOffsetZ;
};

} // namespace inspector

#endif // INSPECTOR_DENSE_EXPORT_H
