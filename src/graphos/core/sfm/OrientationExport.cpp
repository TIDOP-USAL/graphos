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

#include "graphos/core/sfm/orientationexport.h"

#include <colmap/scene/reconstruction.h>
//#include <colmap/util/bitmap.h>
//#include <colmap/util/misc.h>
//#include <colmap/base/camera_database.h>

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Endian.h>
#include <tidop/math/algebra/rotations/RotationMatrix.h>

//#include <QDir>
//#include <QSqlQuery>
//#include <QSqlError>
//#include <QVariant>
//#include <QApplication>

#include <fstream>

//#ifdef _DEBUG
//  /// Borrar
//#include "colmap/base/projection.h"
//#include "colmap/base/point3d.h"
//#endif

//using namespace Qt;

namespace graphos
{

OrientationExport::OrientationExport(const colmap::Reconstruction *reconstruction,
                                     const tl::Point3d &offset)
  : mReconstruction(reconstruction),
    mOffset(offset)
{
}

OrientationExport::~OrientationExport()
{
}

void OrientationExport::exportBinary(const tl::Path &path) const
{
    if (mReconstruction)
        mReconstruction->WriteBinary(path.toString());
    else
        tl::Message::error("There is not a valid reconstruction");
}

void OrientationExport::exportText(const tl::Path &path) const
{
    if (mReconstruction)
        mReconstruction->WriteText(path.toString());
    else
        tl::Message::error("There is not a valid reconstruction");
}

//void OrientationExport::exportNVM(const tl::Path &path) const
//{
//    if (mReconstruction)
//        mReconstruction->ExportNVM(path.toString());
//    else
//        tl::Message::error("There is not a valid reconstruction");
//}

struct PointPly
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

/// Extraido de colmap para filtra los puntos para visualización

void writeBinaryPlyPoints(const std::string &path,
                          const std::vector<PointPly> &points,
                          bool write_normal, bool write_rgb)
{
    std::fstream text_file(path, std::ios::out);
    TL_ASSERT(text_file.is_open(), "File not open: {}", path);

    text_file << "ply\n";
    text_file << "format binary_little_endian 1.0\n";
    text_file << "element vertex " << points.size() << '\n';

    text_file << "property float x\n";
    text_file << "property float y\n";
    text_file << "property float z\n";

    if (write_normal) {
        text_file << "property float nx\n";
        text_file << "property float ny\n";
        text_file << "property float nz\n";
    }

    if (write_rgb) {
        text_file << "property uchar red\n";
        text_file << "property uchar green\n";
        text_file << "property uchar blue\n";
    }

    text_file << "end_header\n";
    text_file.close();

    std::fstream stream(path, std::ios::out | std::ios::binary | std::ios::app);
    if (!stream.is_open()) throw std::runtime_error(std::string("File not open: ").append(path));

    for (const auto &point : points) {
        //colmap::WriteBinaryLittleEndian<float>(&stream, point.x);
        //colmap::WriteBinaryLittleEndian<float>(&stream, point.y);
        //colmap::WriteBinaryLittleEndian<float>(&stream, point.z);
        tl::write(&stream, point.x);
        tl::write(&stream, point.y);
        tl::write(&stream, point.z);

        if (write_normal) {
        //    colmap::WriteBinaryLittleEndian<float>(&stream, point.nx);
        //    colmap::WriteBinaryLittleEndian<float>(&stream, point.ny);
        //    colmap::WriteBinaryLittleEndian<float>(&stream, point.nz);
            tl::write(&stream, point.nx);
            tl::write(&stream, point.ny);
            tl::write(&stream, point.nz);
        }

        if (write_rgb) {
            //colmap::WriteBinaryLittleEndian<uint8_t>(&stream, point.r);
            //colmap::WriteBinaryLittleEndian<uint8_t>(&stream, point.g);
            //colmap::WriteBinaryLittleEndian<uint8_t>(&stream, point.b);
            tl::write(&stream, point.r);
            tl::write(&stream, point.g);
            tl::write(&stream, point.b);
        }
    }

    stream.close();
}

void OrientationExport::exportPLY(const tl::Path &path) const
{
    if (mReconstruction) {

        std::vector<PointPly> ply_points;
        ply_points.reserve(mReconstruction->NumPoints3D());

        for (auto &[_,point] : mReconstruction->Points3D()) {
            if (point.error < 2 &&
                point.track.Length() >= 3) {
                PointPly ply_point;
                ply_point.x = static_cast<float>(point.xyz.x());
                ply_point.y = static_cast<float>(point.xyz.y());
                ply_point.z = static_cast<float>(point.xyz.z());
                ply_point.r = point.color(0);
                ply_point.g = point.color(1);
                ply_point.b = point.color(2);
                ply_points.push_back(ply_point);
            }
        }

        bool kWriteNormal = false;
        bool kWriteRGB = true;
        writeBinaryPlyPoints(path.toString(), ply_points, kWriteNormal, kWriteRGB);

    } else
        tl::Message::error("There is not a valid reconstruction");
}

} // namespace graphos
