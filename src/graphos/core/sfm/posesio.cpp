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

#include <fstream>
#include <colmap/base/reconstruction.h>
#include <tidop/core/endian.h>

#include <tidop/core/path.h>
#include <tidop/math/algebra/quaternion.h>

namespace graphos
{



/* Camera Poses Reader */

CameraPosesReader::CameraPosesReader() = default;

auto CameraPosesReader::cameraPoses() const -> std::unordered_map<size_t, CameraPose>
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

    GraphosCameraPosesReader() = default;

    ~GraphosCameraPosesReader() override = default;

// CameraPosesReader

public:

    void read(const tl::Path &path) override
    {
        try {

            std::fstream stream(path.toString(), std::ios_base::in | std::ios_base::binary);

            TL_ASSERT(stream.is_open(), "File not open: {}", path.toString());

            uint64_t size = 0;
            std::array<char, 19> header_message;
            stream.read(header_message.data(), sizeof(char) * 19);
            tl::read(&stream, size);

            for (size_t i = 0; i < size; i++) {

                size_t image_id = 0;
                tl::read(&stream, image_id);

                CameraPose camera_pose;

                tl::Point3<double> coordinates;

                tl::read(&stream, coordinates.x);
                tl::read(&stream, coordinates.y);
                tl::read(&stream, coordinates.z);

                camera_pose.setPosition(coordinates);

                tl::Quaternion<double> quaternion;
                tl::read(&stream, quaternion.x);
                tl::read(&stream, quaternion.y);
                tl::read(&stream, quaternion.z);
                tl::read(&stream, quaternion.w);

                camera_pose.setQuaternion(quaternion);

                addCameraPose(image_id, camera_pose);

            }

            //TL_TODO("Cambiar la lectura a fstream")

            //FILE *file = std::fopen(path.toString().c_str(), "rb");

            //TL_ASSERT(file, "File not open: {}", path.toString());

            //uint64_t size = 0;

            ///// Header
            //{
            //    std::array<char, 19> header_message;
            //    std::fread(&header_message, sizeof(char), 19, file);
            //    std::fread(&size, sizeof(uint64_t), 1, file);
            //}

            //for (size_t i = 0; i < size; i++) {

            //    size_t image_id = 0;
            //    std::fread(&image_id, sizeof(uint64_t), 1, file);

            //    CameraPose camera_pose;

            //    tl::Point3<double> coordinates;

            //    std::fread(&coordinates.x, sizeof(double), 1, file);
            //    std::fread(&coordinates.y, sizeof(double), 1, file);
            //    std::fread(&coordinates.z, sizeof(double), 1, file);

            //    camera_pose.setPosition(coordinates);

            //    tl::Quaternion<double> quaternion;
            //    std::fread(&quaternion.x, sizeof(double), 1, file);
            //    std::fread(&quaternion.y, sizeof(double), 1, file);
            //    std::fread(&quaternion.z, sizeof(double), 1, file);
            //    std::fread(&quaternion.w, sizeof(double), 1, file);

            //    camera_pose.setQuaternion(quaternion);

            //    addCameraPose(image_id, camera_pose);

            //}

            //std::fclose(file);

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("");
        }
    }

    auto format() const -> std::string final
    {
        return "GRAPHOS_BIN";
    }
};





/* Camera Poses Reader Factory */

auto CameraPosesReaderFactory::create(const std::string& format) -> std::unique_ptr<CameraPosesReader>
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

CameraPosesWriter::CameraPosesWriter() = default;

void CameraPosesWriter::setCameraPoses(const std::unordered_map<size_t, CameraPose> &cameraPoses)
{
    mCameraPoses = cameraPoses;
}

auto CameraPosesWriter::cameraPoses() const -> std::unordered_map<size_t, CameraPose>
{
    return mCameraPoses;
}




/* Camera Poses Writer Graphos */

class GraphosCameraPosesWriter
  : public CameraPosesWriter
{

public:

    GraphosCameraPosesWriter()
    = default;

    ~GraphosCameraPosesWriter() override = default;

// CameraPosesWriter

public:

    void write(const tl::Path &path) override
    {
        try {

;            std::ofstream stream(path.toString(), std::ios_base::trunc | std::ios_base::binary);

            TL_ASSERT(stream.is_open(), "File not open");

            const auto &camera_poses = this->cameraPoses();

            // Header
            {
                stream.write("GRAPHOS_POSES_V1.0", sizeof("GRAPHOS_POSES_V1.0"));
                uint64_t size = camera_poses.size();
                tl::write(&stream, size);
            }

            for (const auto &camera_pose : camera_poses) {

                size_t image_id = camera_pose.first;
                tl::write(&stream, image_id);

                tl::Point3<double> coordinates = camera_pose.second.position();

                tl::write(&stream, coordinates.x);
                tl::write(&stream, coordinates.y);
                tl::write(&stream, coordinates.z);

                tl::Quaternion<double> quaternion = camera_pose.second.quaternion();
                tl::write(&stream, quaternion.x);
                tl::write(&stream, quaternion.y);
                tl::write(&stream, quaternion.z);
                tl::write(&stream, quaternion.w);

            }

            //FILE *file = std::fopen(path.toString().c_str(), "wb");

            //TL_ASSERT(file, "File not open");

            //const auto &camera_poses = this->cameraPoses();

            //// Header
            //{
            //    std::fwrite("GRAPHOS_POSES_V1.0", sizeof(char), 19, file);
            //    uint64_t size = camera_poses.size();
            //    std::fwrite(&size, sizeof(uint64_t), 1, file);
            //}

            //for (const auto &camera_pose : camera_poses) {

            //    size_t image_id = camera_pose.first;
            //    std::fwrite(&image_id, sizeof(uint64_t), 1, file);

            //    tl::Point3<double> coordinates = camera_pose.second.position();

            //    std::fwrite(&coordinates.x, sizeof(double), 1, file);
            //    std::fwrite(&coordinates.y, sizeof(double), 1, file);
            //    std::fwrite(&coordinates.z, sizeof(double), 1, file);

            //    tl::Quaternion<double> quaternion = camera_pose.second.quaternion();
            //    std::fwrite(&quaternion.x, sizeof(double), 1, file);
            //    std::fwrite(&quaternion.y, sizeof(double), 1, file);
            //    std::fwrite(&quaternion.z, sizeof(double), 1, file);
            //    std::fwrite(&quaternion.w, sizeof(double), 1, file);

            //}

            //std::fclose(file);

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
        }
    }

    auto format() const -> std::string final
    {
        return std::string("GRAPHOS_BIN");
    }

};



/* Camera Poses Writer Factory */

auto CameraPosesWriterFactory::create(const std::string& format) -> std::unique_ptr<CameraPosesWriter>
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

tl::Point3<double> offsetRead(const tl::Path &path)
{
    tl::Point3<double> coordinates;

    try {

        std::ifstream stream(path.toString(), std::ios::in);

        TL_ASSERT(stream.is_open(), "File not open: {}", path.toString());

        std::string line;
        std::getline(stream, line);
        auto v = tl::split<double>(line, ' ');

        coordinates.x = v.at(0);
        coordinates.y = v.at(1);
        coordinates.z = v.at(2);

        stream.close();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return coordinates;
}

void offsetWrite(const tl::Path &path, const tl::Point3<double> &coordinates)
{
    try {


    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}
} // namespace graphos
