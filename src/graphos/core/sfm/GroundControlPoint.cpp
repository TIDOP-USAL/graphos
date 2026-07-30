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

#include "graphos/core/sfm/GroundControlPoint.h"

#include <tidop/core/base/Endian.h>
#include <tidop/core/base/Split.h>

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <fstream>


namespace graphos
{

GroundControlPoint::GroundControlPoint()
{
}

GroundControlPoint::GroundControlPoint(const tl::Point3d &point3d)
  : tl::Point3d(point3d)
{
}

GroundControlPoint::~GroundControlPoint() = default;

auto GroundControlPoint::name() const -> std::string
{
    return mName;
}

void GroundControlPoint::setName(const std::string &name)
{
    mName = name;
}

void GroundControlPoint::setPoint(const tl::Point3d &point)
{
    this->x() = point.x();
    this->y() = point.y();
    this->z() = point.z();
}

auto GroundControlPoint::error() const -> double/*tl::Vector3d*/
{
    return mError;
}

void GroundControlPoint::setError(/*const tl::Vector3d &*/double error)
{
    mError = error;
}

void GroundControlPoint::removeTrackPoint(size_t imageId)
{
    mTrack.removePoint(imageId);
}

void GroundControlPoint::addPointToTrack(size_t imageId, const tl::Point<double> &point)
{
    mTrack.addPoint(imageId, point);
}

void GroundControlPoint::addErrorToTrack(size_t imageId, const tl::Vector2d &error)
{
    mTrack.setError(imageId, error);
}

void GroundControlPoint::setTrack(const GCPTrack &track)
{
    mTrack = track;
}

auto GroundControlPoint::track() const -> const GCPTrack&
{
    return mTrack;
}



/* Readers */

GCPsReader::GCPsReader() = default;

auto GCPsReader::gcps() const -> std::vector<GroundControlPoint>
{
    return mGCPs;
}

auto GCPsReader::epsgCode() const -> std::string
{
    return mEpsgCode;
}

void GCPsReader::setImages(const std::unordered_map<size_t, Image> &images)
{
    mImages = images;
}

void GCPsReader::addGroundControlPoint(const GroundControlPoint &gcp)
{
    mGCPs.push_back(gcp);
}

void GCPsReader::setEPSGCode(const std::string &epsgCode)
{
    mEpsgCode = epsgCode;
}

auto GCPsReader::images() const -> const std::unordered_map<size_t, Image>&
{
    return mImages;
}




/* Graphos reader */

class GraphosGCPsReader
  : public GCPsReader
{

public:

    GraphosGCPsReader() = default;
    ~GraphosGCPsReader() override = default;

    // GCPsReader

    void read(const tl::Path &path) override
    {
        try {

            QString gcp_file = QString::fromStdWString(path.toWString());
            QFile file(gcp_file);
            if (file.open(QFile::ReadOnly)) {

                QXmlStreamReader stream;
                stream.setDevice(&file);

                if (stream.readNextStartElement()) {
                    if (stream.name() == "Graphos") {
                        while (stream.readNextStartElement()) {
                            if (stream.name() == "GroundControlPoints") {
                                while (stream.readNextStartElement()) {
                                    if (stream.name() == "Crs") {
                                        setEPSGCode(stream.readElementText().toStdString());
                                    } else if (stream.name() == "GroundControlPoint") {
                                        addGroundControlPoint(readGroundControlPoint(stream));
                                    } else
                                        stream.skipCurrentElement();
                                }
                            } else
                                stream.skipCurrentElement();
                        }
                    } else {
                        stream.raiseError(QObject::tr("Incorrect project file"));
                    }

                    file.close();
                }
            }

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("Ground Control Point read error");
        }
    }

    auto format() const -> std::string final
    {
        return "GRAPHOS";
    }

private:

    static auto readGroundControlPoint(QXmlStreamReader &stream) -> GroundControlPoint
    {
        GroundControlPoint gcp;
        GCPTrack track;
        QString error;
        while (stream.readNextStartElement()) {
            if (stream.name() == "Name") {
                gcp.setName(stream.readElementText().toStdString());
            } else if (stream.name() == "x") {
                gcp.x() = stream.readElementText().toDouble();
            } else if (stream.name() == "y") {
                gcp.y() = stream.readElementText().toDouble();
            } else if (stream.name() == "z") {
                gcp.z() = stream.readElementText().toDouble();
            } else if (stream.name() == "error") {
                gcp.setError(stream.readElementText().toDouble());
            } else if (stream.name() == "ImagePoints") {

                while (stream.readNextStartElement()) {

                    if (stream.name() == "ImagePoint") {

                        size_t image_id = 0;
                        tl::Point<double> point_2d;
                        tl::Vector2d error;
                        for (auto &attr : stream.attributes()) {
                            if (attr.name().compare(QString("image_id")) == 0) {
                                image_id = attr.value().toULongLong();
                                break;
                            }
                        }

                        while (stream.readNextStartElement()) {
                            if (stream.name() == "x") {
                                point_2d.x() = stream.readElementText().toDouble();
                            } else if (stream.name() == "y") {
                                point_2d.y() = stream.readElementText().toDouble();
                            } else if (stream.name() == "ex") {
                                error[0] = stream.readElementText().toDouble();
                            } else if (stream.name() == "ey") {
                                error[1] = stream.readElementText().toDouble();
                            } else {
                                stream.skipCurrentElement();
                            }
                        }

                        track.addPoint(image_id, point_2d);
                        track.setError(image_id, error);

                    } else
                        stream.skipCurrentElement();

                }

            } else {
                stream.skipCurrentElement();
            }
        }

        gcp.setTrack(track);

        return gcp;
    }
};


class OpenDroneMapGCPsReader
  : public GCPsReader
{

public:
    
    OpenDroneMapGCPsReader()
    = default;

    ~OpenDroneMapGCPsReader() override = default;

// GCPsReader interfaces

public:

    void read(const tl::Path &path) override
    {
        std::ifstream ifs;
        ifs.open(path.toString(), std::ifstream::in);

        TL_ASSERT(ifs.is_open(), "File not open: {}", path.toString());
        TL_ASSERT(!images().empty(), "");

        std::string line;
        std::getline(ifs, line);

        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());

        std::string projection = line;
        setEPSGCode(projection);
        ///TODO: hay que ver si la proyección viene por codigo EPSG o en formato PROJ

        std::vector<GroundControlPoint> ground_control_points;

        std::stringstream ss;

        int counter = 0;
        while (std::getline(ifs, line)) {

            if (line.empty()) continue;

            ss << line;

            tl::Point3d point;
            tl::Point<double> image_point;
            std::string image_name;
            ss >> point.x() >> point.y() >> point.z() >> image_point.x() >> image_point.y() >> image_name;
            ss.clear();

            size_t image_id = 0;

            for (auto &image : images()) {

                if (image.second.name().toStdString() == image_name) {
                    image_id = image.first;
                    break;
                }
            }

            if (image_id != 0) {

                bool exists_gcp = false;
                for (auto &ground_control_point : ground_control_points) {
                    if (ground_control_point.x() == point.x() &&
                        ground_control_point.y() == point.y() &&
                        ground_control_point.z() == point.z()) {

                        ground_control_point.addPointToTrack(image_id, image_point);

                        exists_gcp = true;
                        break;
                    }
                }

                if (!exists_gcp) {
                    GroundControlPoint gcp(point);
                    gcp.setName(std::string("P").append(std::to_string(counter++)));
                    gcp.addPointToTrack(image_id, image_point);
                    ground_control_points.push_back(gcp);
                }

            }
        }

        ifs.close();

        for (auto &ground_control_point : ground_control_points) {
            addGroundControlPoint(ground_control_point);
        }

    }

    auto format() const -> std::string final
    {
        return "ODM";
    }

};




auto GCPsReaderFactory::create(const std::string& format) -> std::unique_ptr<GCPsReader>
{
    std::unique_ptr<GCPsReader> reader;

    try {

        if (format == "GRAPHOS") {
            reader = std::make_unique<GraphosGCPsReader>();
        } else if (format == "ODM") {
            reader = std::make_unique<OpenDroneMapGCPsReader>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return reader;
}




/* Writers */

GCPsWriter::GCPsWriter() = default;

void GCPsWriter::setGCPs(const std::vector<GroundControlPoint> &GCPs)
{
    mGCPs = GCPs;
}

void GCPsWriter::setEPSGCode(const std::string &epsgCode)
{
    mEpsgCode = epsgCode;
}

void GCPsWriter::setImages(const std::unordered_map<size_t,Image>& images)
{
    mImages = images;
}

auto GCPsWriter::gcps() const -> const std::vector<GroundControlPoint>&
{
    return mGCPs;
}

auto GCPsWriter::epsgCode() const -> const std::string&
{
    return mEpsgCode;
}

auto GCPsWriter::images() const -> const std::unordered_map<size_t, Image>&
{
    return mImages;
}



/* Graphos writer */

class GraphosGCPsWriter
  : public GCPsWriter
{

public:

    GraphosGCPsWriter() = default;
    ~GraphosGCPsWriter() override = default;

// GCPsWriter

    void write(const tl::Path &path) override
    {
        try {

            QString gcp_file = QString::fromStdWString(path.toWString());
            QFile file(gcp_file);

            if (file.open(QFile::WriteOnly)) {

                QXmlStreamWriter stream(&file);
                stream.setAutoFormatting(true);
                stream.writeStartDocument();

                stream.writeStartElement("Graphos");
                writeGroundControlPoints(stream);
                stream.writeEndElement();

                file.close();
            }

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("");
        }
    }

    void writeGroundControlPoints(QXmlStreamWriter &stream) const
    {
        try {

            stream.writeStartElement("GroundControlPoints");
            {
                stream.writeTextElement("Crs", QString::fromStdString(epsgCode()));

                for (const auto &gcp : gcps()) {


                    stream.writeStartElement("GroundControlPoint");
                    stream.writeTextElement("Name", QString::fromStdString(gcp.name()));
                    stream.writeTextElement("x", QString::number(gcp.x(), 'f', 6));
                    stream.writeTextElement("y", QString::number(gcp.y(), 'f', 6));
                    stream.writeTextElement("z", QString::number(gcp.z(), 'f', 6));
                    stream.writeTextElement("error", QString::number(gcp.error(), 'f', 3));
                    stream.writeStartElement("ImagePoints");

                    for (const auto &point : gcp.track().points()) {

                        stream.writeStartElement("ImagePoint");
                        stream.writeAttribute("image_id", QString::number(point.first));
                        stream.writeTextElement("x", QString::number(point.second.x()));
                        stream.writeTextElement("y", QString::number(point.second.y()));
                        stream.writeTextElement("ex", QString::number(gcp.track().error(point.first).x()));
                        stream.writeTextElement("ey", QString::number(gcp.track().error(point.first).y()));
                        stream.writeEndElement();

                    }

                    stream.writeEndElement();
                    stream.writeEndElement();
                }
            }
            stream.writeEndElement();

        } catch (...) {
            TL_THROW_EXCEPTION_WITH_NESTED("");
        }
    }

    auto format() const -> std::string final
    {
        return "GRAPHOS";
    }

};


class OpenDroneMapGCPsWriter
    : public GCPsWriter
{

public:
    
    OpenDroneMapGCPsWriter() = default;

    ~OpenDroneMapGCPsWriter() override = default;

// GCPsWriter interface

public:

    void write(const tl::Path &path) override
    {
        std::ofstream stream(path.toString(), std::ios::trunc);
        TL_ASSERT(stream.is_open(), "Can't open {}", path.toString());

        stream << epsgCode() << '\n';

        stream << std::fixed << std::setprecision(6);

        for (const auto &gcp : gcps()) {

            for (const auto &point : gcp.track().points()) {

                stream << gcp.x() << " " << gcp.y() << " " << gcp.z() << " " << point.second.x() << " " << point.second.y() << " " << images().at(point.first).name().toStdString() <<
                    '\n';
            }
        }

        stream.close();
    }

    auto format() const -> std::string final
    {
        return "ODM";
    }

};



/* GCPsWriterFactory */

auto GCPsWriterFactory::create(const std::string &format) -> std::unique_ptr<GCPsWriter>
{
    std::unique_ptr<GCPsWriter> writer;

    try {

        if (format == "GRAPHOS") {
            writer = std::make_unique<GraphosGCPsWriter>();
        } else if (format == "ODM") {
            writer = std::make_unique<OpenDroneMapGCPsWriter>();
        } else {
            TL_THROW_EXCEPTION("Invalid format: {}", format);
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return writer;
}

}
