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

#include "graphos/core/sfm/CameraPosesWriter.h"

#include <tidop/core/base/Endian.h>
#include <tidop/core/base/Path.h>
#include <tidop/math/algebra/rotations/Quaternion.h>

//#include <colmap/base/reconstruction.h>

#include <fstream>

namespace graphos
{

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

    GraphosCameraPosesWriter() = default;

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

                tl::Point3d coordinates = camera_pose.second.position();

                tl::write(&stream, coordinates.x());
                tl::write(&stream, coordinates.y());
                tl::write(&stream, coordinates.z());

                tl::Quaternion<double> quaternion = camera_pose.second.quaternion();
                tl::write(&stream, quaternion.x());
                tl::write(&stream, quaternion.y());
                tl::write(&stream, quaternion.z());
                tl::write(&stream, quaternion.w());

            }

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

} // namespace graphos
