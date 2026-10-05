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

#include "graphos/core/orientation/io/GCPsWriter.h"

#include <tidop/core/base/Endian.h>
#include <tidop/core/base/Split.h>

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <fstream>


namespace graphos
{

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
