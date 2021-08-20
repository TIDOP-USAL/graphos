#include "DenseExport.h"

#include <colmap/util/ply.h>

#include <fstream>
#include <iomanip>

namespace graphos
{

DenseExport::DenseExport(const std::string &denseModel)
  : mDenseModel(denseModel),
    mOffset()
{

}

void DenseExport::setOffset(const tl::Point3<double> &point)
{
  mOffset = point;
}

void DenseExport::exportToCSV(const std::string &csv,
                              const tl::EnumFlags<DenseExport::Fields> &flag,
                              tl::BoundingBox<tl::Point3<double>> *bbox)
{
  //std::vector<colmap::PlyPoint> points = colmap::ReadPly(mDenseModel.toStdString());

  std::ofstream stream(csv, std::ios::trunc);

  if (stream.is_open()){

    tl::Point3<double> point;

    stream << "X;Y;Z";
    if (flag.isActive(Fields::rgb)){
      stream << ";R;G;B";
    }

    if (flag.isActive(Fields::normals)) {
      stream << ";Nx;Ny;Nz";
    }
    stream << std::endl;

    std::vector<colmap::PlyPoint> points = colmap::ReadPly(mDenseModel);

    //tl::BoundingBox<tl::Point3<double>> bounding_box;

    for (size_t i = 0; i < points.size(); i++){

      point.x = points[i].x;
      point.y = points[i].y;
      point.z = points[i].z;

      point += mOffset;

      if (bbox) {
        bbox->pt1.x = std::min(bbox->pt1.x, point.x);
        bbox->pt1.y = std::min(bbox->pt1.y, point.y);
        bbox->pt1.z = std::min(bbox->pt1.z, point.z);
        bbox->pt2.x = std::max(bbox->pt2.x, point.x);
        bbox->pt2.y = std::max(bbox->pt2.y, point.y);
        bbox->pt2.z = std::max(bbox->pt2.z, point.z);
      }

      stream << std::fixed << std::setprecision(3)
             << point.x << ";"
             << point.y << ";"
             << point.z;

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

} // namespace graphos
