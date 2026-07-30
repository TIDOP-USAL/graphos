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

#include "graphos/core/sfm/CameraPosesReader.h"

#include <tidop/core/base/Endian.h>
#include <tidop/core/base/Path.h>
#include <tidop/math/algebra/rotations/Quaternion.h>

//#include <colmap/base/reconstruction.h>

#include <fstream>

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

                tl::Point3d coordinates;

                tl::read(&stream, coordinates.x());
                tl::read(&stream, coordinates.y());
                tl::read(&stream, coordinates.z());

                camera_pose.setPosition(coordinates);

                tl::Quaternion<double> quaternion;
                tl::read(&stream, quaternion.x());
                tl::read(&stream, quaternion.y());
                tl::read(&stream, quaternion.z());
                tl::read(&stream, quaternion.w());

                camera_pose.setQuaternion(quaternion);

                addCameraPose(image_id, camera_pose);

            }

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

} // namespace graphos
