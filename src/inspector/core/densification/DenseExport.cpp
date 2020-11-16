#include "DenseExport.h"

#include <colmap/util/ply.h>

#include <tidop/geometry/entities/point.h>

#include <fstream>

namespace inspector
{

DenseExport::DenseExport(const QString &denseModel)
  : mDenseModel(denseModel)
{

}

void DenseExport::setOffset(double x, double y, double z)
{
  mOffsetX = x;
  mOffsetY = y;
  mOffsetZ = z;
}

void DenseExport::exportToCSV(const QString &csv,
                              const tl::EnumFlags<DenseExport::Fields> &flag)
{
  //std::vector<colmap::PlyPoint> points = colmap::ReadPly(mDenseModel.toStdString());

  std::ofstream stream(csv.toStdString(), std::ios::trunc);
  if (stream.is_open()){
    tl::Point3<double> offset(mOffsetX, mOffsetY, mOffsetZ);
    tl::Point3<double> point;

    stream << "X;Y;Z";
    if (flag.isActive(Fields::rgb)){
      stream << ";R;G;B";
    }

    if (flag.isActive(Fields::normals)) {
      stream << ";Nx;Ny;Nz";
    }
    stream << std::endl;

    std::vector<colmap::PlyPoint> points = colmap::ReadPly(mDenseModel.toStdString());

    for (size_t i = 0; i < points.size(); i++){

      point.x = points[i].x;
      point.y = points[i].y;
      point.z = points[i].z;

      point += offset;

      stream << QString::number(point.x, 'f', 3).toStdString() << ";"
             << QString::number(point.y, 'f', 3).toStdString() << ";"
             << QString::number(point.z, 'f', 3).toStdString();

      if (flag.isActive(Fields::rgb)){
        stream << ";" <<
                  static_cast<int>(points[i].r) << ";" <<
                  static_cast<int>(points[i].g) << ";" <<
                  static_cast<int>(points[i].b);
      }

      if (flag.isActive(Fields::normals)) {
        stream << ";" <<
                  points[i].nx << ";" <<
                  points[i].ny << ";" <<
                  points[i].nz;
      }

      stream << std::endl;
    }

    stream.close();
  }
}

} // namespace inspector
