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

#include "graphos/core/orientation/GroundPoint.h"

#include <tidop/core/base/Endian.h>
#include <tidop/core/base/Split.h>

#include <fstream>


namespace graphos
{


/* Ground Points */

GroundPoint::GroundPoint() = default;

GroundPoint::GroundPoint(const tl::Point3d &point3d)
  : tl::Point3d(point3d)
{
}

GroundPoint::~GroundPoint() = default;

void GroundPoint::setPoint(const tl::Point3d &point)
{
    this->x() = point.x();
    this->y() = point.y();
    this->z() = point.z();
}

auto GroundPoint::color() const -> tl::Color
{
    return mColor;
}

void GroundPoint::addPairToTrack(size_t imageId,
                                 size_t pointId)
{
    mTrack.addPair(imageId, pointId);
}

void GroundPoint::setColor(const tl::Color &color)
{
    mColor = color;
}

void GroundPoint::setTrack(const Track &track)
{
    mTrack = track;
}

void GroundPoint::removeTrackPair(size_t imageId)
{
    mTrack.removePair(imageId);
}

auto GroundPoint::track() const -> const Track&
{
    return mTrack;
}



/* GroundPointsReader */


GroundPointsReader::GroundPointsReader() = default;

auto GroundPointsReader::points() const -> std::vector<GroundPoint>
{
    return mGroundPoints;
}

auto GroundPointsReader::epsgCode() const -> std::string
{
    return mEpsgCode;
}

void GroundPointsReader::addGroundPoint(const GroundPoint &groundPoint)
{
    mGroundPoints.push_back(groundPoint);
}

void GroundPointsReader::setGroundPoints(const std::vector<GroundPoint> &groundPoint)
{
    mGroundPoints = groundPoint;
}

void GroundPointsReader::setEPSGCode(const std::string &epsgCode)
{
    mEpsgCode = epsgCode;
}


/* GraphosGPsReader */

class GraphosGPsReader
  : public GroundPointsReader
{

public:

    GraphosGPsReader() = default;
    ~GraphosGPsReader() override = default;

// GroundPointsReader

public:

    void read(const tl::Path &path) override
    {
        try {

            TL_ASSERT(path.exists(), "File not exists");

            std::fstream stream(path.toString(), std::ios_base::in | std::ios_base::binary);

            if (stream.is_open()) {

                uint32_t epsg;
                tl::read(&stream, epsg);
                if (epsg) {
                    std::string code;
                    code.append("EPSG:");
                    code.append(std::to_string(epsg));
                    this->setEPSGCode(code);
                }

                uint64_t size = 0;
                tl::read(&stream, size);
                std::vector<graphos::GroundPoint> ground_points(size);

                for (auto &ground_point : ground_points) {

                    tl::read(&stream, ground_point.x());
                    tl::read(&stream, ground_point.y());
                    tl::read(&stream, ground_point.z());

                    uint32_t color = 0;
                    tl::read(&stream, color);
                    if (color)
                        ground_point.setColor(tl::Color::fromRGB(color));

                    size = 0;
                    tl::read(&stream, size);

                    Track track;

                    size_t image_id = 0;
                    size_t point_id = 0;

                    for (size_t i = 0; i < size; i++) {

                        tl::read(&stream, image_id);
                        tl::read(&stream, point_id);

                        track.addPair(image_id, point_id);

                    }

                    ground_point.setTrack(track);

                }

                this->setGroundPoints(ground_points);
            }

            stream.close();

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("");
        }

    }

    auto format() const -> std::string final
    {
        return std::string("GRAPHOS");
    }

};


/* GroundPointsReaderFactory */

auto GroundPointsReaderFactory::create(const std::string& format) -> std::unique_ptr<GroundPointsReader>
{
    std::unique_ptr<GroundPointsReader> reader;

    try {

        if (format == "GRAPHOS") {
            reader = std::make_unique<GraphosGPsReader>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return reader;
}


/* GroundPointsWriter */

GroundPointsWriter::GroundPointsWriter()
{
}

void GroundPointsWriter::setGroundPoints(const std::vector<GroundPoint> &groundPoint)
{
    mGroundPoints = groundPoint;
}

void GroundPointsWriter::setEPSGCode(const std::string &epsgCode)
{
    mEpsgCode = epsgCode;
}

auto GroundPointsWriter::groundPoints() const -> std::vector<GroundPoint>
{
    return mGroundPoints;
}

auto GroundPointsWriter::epsgCode() const -> std::string
{
    return mEpsgCode;
}


class GraphosGPsWriter
  : public GroundPointsWriter
{
public:

    GraphosGPsWriter() = default;

    ~GraphosGPsWriter() override = default;

// GroundPointsWriter

    void write(const tl::Path &path) override
    {
        try {

            std::ofstream stream(path.toString(), std::ios_base::trunc | std::ios_base::binary);

            TL_ASSERT(stream.is_open(), "File not open");

            uint32_t epsg = 0;
            std::string epsg_code = this->epsgCode();
            if (!epsg_code.empty()) {
                auto split_string = tl::split<std::string>(epsg_code, ':');
                if (split_string.size() == 2) {
                    epsg = std::stoi(split_string[1]);
                }
            }

            tl::write(&stream, epsg);

            uint64_t size = this->groundPoints().size();
            tl::write(&stream, size);

            for (auto &ground_point : this->groundPoints()) {

                tl::write(&stream, ground_point.x());
                tl::write(&stream, ground_point.y());
                tl::write(&stream, ground_point.z());

                uint32_t color = ground_point.color().rgb();

                tl::write(&stream, color);

                const auto &track = ground_point.track();
                size = track.size();
                tl::write(&stream, size);

                for (const auto &pair : track.pairs()) {
                    tl::write(&stream, pair.first);
                    tl::write(&stream, pair.second);
                }
            }

            stream.close();

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
        }

    }

    auto format() const -> std::string final
    {
        return std::string("GRAPHOS");
    }

};


/* GroundPointsWriterFactory */

auto GroundPointsWriterFactory::create(const std::string &format) -> std::unique_ptr<GroundPointsWriter>
{
    std::unique_ptr<GroundPointsWriter> writer;

    try {

        if (format == "GRAPHOS") {
            writer = std::make_unique<GraphosGPsWriter>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return writer;
}

}
