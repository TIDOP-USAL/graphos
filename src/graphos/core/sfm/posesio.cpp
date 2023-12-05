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

#include "graphos/core/sfm/posesio.h"

#include <colmap/base/reconstruction.h>

#include <tidop/core/path.h>
#include <tidop/math/algebra/quaternion.h>

namespace graphos
{

//ReadCameraPoses::ReadCameraPoses()
//  : mReconstruction(new colmap::Reconstruction)
//{
//
//}
//
//ReadCameraPoses::~ReadCameraPoses()
//{
//    if (mReconstruction) {
//        delete mReconstruction;
//        mReconstruction = nullptr;
//    }
//}
//
//void ReadCameraPoses::open(const QString &path)
//{
//    mReconstruction->ReadBinary(path.toStdString());
//}
//
//CameraPose ReadCameraPoses::orientation(const QString &imageName) const
//{
//    CameraPose photoOrientation;
//
//    for (auto &image : mReconstruction->Images()) {
//        std::string image_name = tl::Path(image.second.Name()).fileName().toString();
//        if (imageName.compare(image_name.c_str()) == 0) {
//
//            colmap::image_t image_id = image.second.ImageId();
//            colmap::Image &colmap_image = mReconstruction->Image(image_id);
//
//            const Eigen::Matrix<double, 3, 4> inv_proj_matrix = colmap_image.InverseProjectionMatrix();
//            const Eigen::Vector3d pc = inv_proj_matrix.rightCols<1>();
//            photoOrientation.setPosition(tl::Point3D(pc(0), pc(1), pc(2)));
//
//            Eigen::Matrix3d rot = colmap_image.RotationMatrix();
//            tl::math::RotationMatrix<double> rotation_matrix;
//            rotation_matrix.at(0, 0) = rot(0, 0);
//            rotation_matrix.at(0, 1) = rot(0, 1);
//            rotation_matrix.at(0, 2) = rot(0, 2);
//            rotation_matrix.at(1, 0) = rot(1, 0);
//            rotation_matrix.at(1, 1) = rot(1, 1);
//            rotation_matrix.at(1, 2) = rot(1, 2);
//            rotation_matrix.at(2, 0) = rot(2, 0);
//            rotation_matrix.at(2, 1) = rot(2, 1);
//            rotation_matrix.at(2, 2) = rot(2, 2);
//            photoOrientation.setRotationMatrix(rotation_matrix);
//        }
//    }
//
//    return photoOrientation;
//}


/* Camera Poses Reader */

CameraPosesReader::CameraPosesReader()
{
}

std::unordered_map<size_t, CameraPose> CameraPosesReader::cameraPoses() const
{
    return mCameraPoses;
}

void CameraPosesReader::addCameraPose(size_t imageId,
                                      const CameraPose &cameraPoses)
{
    mCameraPoses[imageId] = cameraPoses;
}


/* Graphos format */

class GraphosCameraPosesReader
  : public CameraPosesReader
{

public:

    GraphosCameraPosesReader()
    {
    }

    ~GraphosCameraPosesReader()
    {
    }

// CameraPosesReader

public:

    void read(const tl::Path &path) override
    {
        try {

            TL_TODO("Cambiar la lectura a fstream")

            FILE *file = std::fopen(path.toString().c_str(), "rb");

            TL_ASSERT(file, "File not open: {}", path.toString());

            uint64_t size = 0;

            /// Header
            {
                std::array<char, 19> header_message;
                std::fread(&header_message, sizeof(char), 19, file);
                std::fread(&size, sizeof(uint64_t), 1, file);
            }

            for (size_t i = 0; i < size; i++) {

                size_t image_id = 0;
                std::fread(&image_id, sizeof(uint64_t), 1, file);

                CameraPose camera_pose;

                tl::Point3<double> coordinates;

                std::fread(&coordinates.x, sizeof(double), 1, file);
                std::fread(&coordinates.y, sizeof(double), 1, file);
                std::fread(&coordinates.z, sizeof(double), 1, file);

                camera_pose.setPosition(coordinates);

                tl::Quaternion<double> quaternion;
                std::fread(&quaternion.x, sizeof(double), 1, file);
                std::fread(&quaternion.y, sizeof(double), 1, file);
                std::fread(&quaternion.z, sizeof(double), 1, file);
                std::fread(&quaternion.w, sizeof(double), 1, file);

                camera_pose.setQuaternion(quaternion);

                addCameraPose(image_id, camera_pose);

            }

            std::fclose(file);

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("");
        }
    }

    std::string format() const final override
    {
        return "GRAPHOS_BIN";
    }
};

/* Camera Poses Reader Factory */

std::unique_ptr<CameraPosesReader> CameraPosesReaderFactory::create(const std::string &format)
{
    std::unique_ptr<CameraPosesReader> reader;

    try {

        if (format == "GRAPHOS") {
            reader = std::make_unique<GraphosCameraPosesReader>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return reader;
}


/* Camera Poses Writer */


CameraPosesWriter::CameraPosesWriter()
{
}

void CameraPosesWriter::setCameraPoses(const std::unordered_map<size_t, CameraPose> &cameraPoses)
{
    mCameraPoses = cameraPoses;
}

std::unordered_map<size_t, CameraPose> CameraPosesWriter::cameraPoses() const
{
    return mCameraPoses;
}


/* Camera Poses Writer Graphos */

class GraphosCameraPosesWriter
    : public CameraPosesWriter
{

public:

    GraphosCameraPosesWriter()
    {
    }

    ~GraphosCameraPosesWriter()
    {
    }

    // CameraPosesWriter

public:

    void write(const tl::Path &path) override
    {
        try {

            FILE *file = std::fopen(path.toString().c_str(), "wb");

            TL_ASSERT(file, "File not open");

            const auto &camera_poses = this->cameraPoses();

            // Header
            {
                std::fwrite("GRAPHOS_POSES_V1.0", sizeof(char), 19, file);
                uint64_t size = camera_poses.size();
                std::fwrite(&size, sizeof(uint64_t), 1, file);
            }

            for (const auto &camera_pose : camera_poses) {

                size_t image_id = camera_pose.first;
                std::fwrite(&image_id, sizeof(uint64_t), 1, file);

                tl::Point3<double> coordinates = camera_pose.second.position();

                std::fwrite(&coordinates.x, sizeof(double), 1, file);
                std::fwrite(&coordinates.y, sizeof(double), 1, file);
                std::fwrite(&coordinates.z, sizeof(double), 1, file);

                tl::Quaternion<double> quaternion = camera_pose.second.quaternion();
                std::fwrite(&quaternion.x, sizeof(double), 1, file);
                std::fwrite(&quaternion.y, sizeof(double), 1, file);
                std::fwrite(&quaternion.z, sizeof(double), 1, file);
                std::fwrite(&quaternion.w, sizeof(double), 1, file);

            }

            std::fclose(file);

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
        }
    }

    virtual std::string format() const final override
    {
        return std::string("GRAPHOS_BIN");
    }

};



/* Camera Poses Writer Factory */

std::unique_ptr<CameraPosesWriter> CameraPosesWriterFactory::create(const std::string &format)
{
    std::unique_ptr<CameraPosesWriter> writer;

    try {

        if (format == "GRAPHOS") {
            writer = std::make_unique<GraphosCameraPosesWriter>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return writer;
}

} // namespace graphos
