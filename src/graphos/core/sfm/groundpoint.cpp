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

#include "graphos/core/sfm/groundpoint.h"

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace graphos
{



GroundControlPoint::GroundControlPoint()
  : tl::Point3<double>()
{

}

GroundControlPoint::GroundControlPoint(const tl::Point3<double> &point3d)
  : tl::Point3<double>(point3d)
{
}

GroundControlPoint::~GroundControlPoint()
{
}

std::string GroundControlPoint::name() const
{
  return mName;
}

void GroundControlPoint::setName(const std::string &name)
{
  mName = name;
}

void GroundControlPoint::setPoint(const tl::Point3<double> &point)
{
  this->x = point.x;
  this->y = point.y;
  this->z = point.z;
}

void GroundControlPoint::removeTrackPoint(size_t imageId)
{
  mTrack.removePoint(imageId);
}

void GroundControlPoint::addPointToTrack(size_t imageId, const tl::Point<double> &point)
{
  mTrack.addPoint(imageId, point);
}

void GroundControlPoint::setTrack(const GCPTrack &track)
{
  mTrack = track;
}

const GCPTrack &GroundControlPoint::track() const
{
  return mTrack;
}



/* Readers */

GCPsReader::GCPsReader()
{
}

std::vector<GroundControlPoint> GCPsReader::gcps() const
{
  return mGCPs;
}

std::string GCPsReader::epsgCode() const
{
  return mEpsgCode;
}

void GCPsReader::addGroundControlPoint(const GroundControlPoint &gcp)
{
  mGCPs.push_back(gcp);
}

void GCPsReader::setEPSGCode(const std::string &epsgCode)
{
  mEpsgCode = epsgCode;
}


/* Graphos reader */

class GraphosGCPsReader
  : public GCPsReader
{

public:

  GraphosGCPsReader()
  {
  }

  ~GraphosGCPsReader() override
  {
  }

// GCPsReader

  void read(const tl::Path &path) override
  {
    try {

      QString gcp_file = QString::fromStdWString(path.toWString());
      QFile file(gcp_file);
      TL_ASSERT(file.open(QFile::ReadOnly), "Open error");

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

                  GroundControlPoint gcp;
                  GCPTrack track;

                  while (stream.readNextStartElement()) {
                    if (stream.name() == "Name") {
                      gcp.setName(stream.readElementText().toStdString());
                    } else if (stream.name() == "x") {
                      gcp.x = stream.readElementText().toDouble();
                    } else if (stream.name() == "y") {
                      gcp.y = stream.readElementText().toDouble();
                    } else if (stream.name() == "z") {
                      gcp.z = stream.readElementText().toDouble();
                    } else if (stream.name() == "error") {
                      QString error = stream.readElementText();
                    } else if (stream.name() == "ImagePoints") {

                      while (stream.readNextStartElement()) {

                        if (stream.name() == "ImagePoint") {


                          size_t image_id = 0;
                          tl::PointD point_2d;

                          for (auto &attr : stream.attributes()) {
                            if (attr.name().compare(QString("image_id")) == 0) {
                              image_id = attr.value().toULongLong();
                              break;
                            }
                          }

                          while (stream.readNextStartElement()) {
                            if (stream.name() == "x") {
                              point_2d.x = stream.readElementText().toDouble();
                            } else if (stream.name() == "y") {
                              point_2d.y = stream.readElementText().toDouble();
                            } else {
                              stream.skipCurrentElement();
                            }
                          }

                          track.addPoint(image_id, point_2d);

                        } else
                          stream.skipCurrentElement();

                      }

                    } else {
                      stream.skipCurrentElement();
                    }
                  }

                  gcp.setTrack(track);
                  addGroundControlPoint(gcp);

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

    } catch (...) {
      TL_THROW_EXCEPTION_WITH_NESTED("Ground Control Point read error");
    }
  }

  std::string format() const final override
  {
    return "GRAPHOS";
  }

};

std::unique_ptr<GCPsReader> GCPsReaderFactory::create(const std::string &format)
{
  std::unique_ptr<GCPsReader> reader;

  try {

    if (format == "GRAPHOS") 	{
      reader = std::make_unique<GraphosGCPsReader>();
    } else {
      TL_THROW_EXCEPTION("Invalid format: %s", format.c_str());
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return reader;
}


/* Writers */

GCPsWriter::GCPsWriter()
{
}

void GCPsWriter::setGCPs(const std::vector<GroundControlPoint> &GCPs)
{
  mGCPs = GCPs;
}

void GCPsWriter::setEPSGCode(const std::string &epsgCode)
{
  mEpsgCode = epsgCode;
}

std::vector<GroundControlPoint> GCPsWriter::gcps() const
{
  return mGCPs;
}

std::string GCPsWriter::epsgCode() const
{
  return mEpsgCode;
}



/* Graphos writer */

class GraphosGCPsWriter
  : public GCPsWriter
{

public:

  GraphosGCPsWriter()
  {
  }

  ~GraphosGCPsWriter()
  {
  }

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

  void writeGroundControlPoints(QXmlStreamWriter &stream)
  {
    try {
      stream.writeStartElement("GroundControlPoints");
      {
        stream.writeTextElement("Crs", QString::fromStdString(epsgCode()));
        
        for (const auto &gcp : gcps()) {
    
     
          stream.writeStartElement("GroundControlPoint");
          stream.writeTextElement("Name", QString::fromStdString(gcp.name()));
          stream.writeTextElement("x", QString::number(gcp.x));
          stream.writeTextElement("y", QString::number(gcp.y));
          stream.writeTextElement("z", QString::number(gcp.z));
          stream.writeTextElement("error", "");
          stream.writeStartElement("ImagePoints");
     
          for (const auto &point : gcp.track().points()) {
     
            stream.writeStartElement("ImagePoint");
            stream.writeAttribute("image_id", QString::number(point.first));
            stream.writeTextElement("x", QString::number(point.second.x));
            stream.writeTextElement("y", QString::number(point.second.y));
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

  std::string format() const final override
  {
    return "GRAPHOS";
  }

};


/* GCPsWriterFactory */

std::unique_ptr<GCPsWriter> GCPsWriterFactory::create(const std::string &format)
{
  std::unique_ptr<GCPsWriter> writer;

  try {

    if (format == "GRAPHOS") {
      writer = std::make_unique<GraphosGCPsWriter>();
    } else {
      TL_THROW_EXCEPTION("Invalid format: %s", format.c_str());
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return writer;
}




/* Ground Points */

GroundPoint::GroundPoint()
{
}

GroundPoint::GroundPoint(const tl::Point3<double> &point3d)
  : tl::Point3<double>(point3d)
{
}

GroundPoint::~GroundPoint()
{
}

void GroundPoint::setPoint(const tl::Point3<double> &point)
{
  this->x = point.x;
  this->y = point.y;
  this->z = point.z;
}

tl::graph::Color GroundPoint::color() const
{
  return mColor;
}

void GroundPoint::addPairToTrack(size_t imageId, 
                                 size_t pointId)
{
  mTrack.addPair(imageId, pointId);
}

void GroundPoint::setColor(const tl::graph::Color &color)
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

const Track &GroundPoint::track() const
{
  return mTrack;
}


/* GroundPointsReader */

GroundPointsReader::GroundPointsReader()
{
}

std::vector<GroundPoint> GroundPointsReader::points() const
{
  return mGroundPoints;
}

std::string GroundPointsReader::epsgCode() const
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

  GraphosGPsReader()
  {
  }

  ~GraphosGPsReader()
  {
  }

// GroundPointsReader

public:

  void read(const tl::Path &path) override
  {
    try {

      FILE *file = std::fopen(path.toString().c_str(), "rb");

      TL_ASSERT(file, "File not open");

      setEPSGCode(readEpsg(file));
      uint64_t size = readGroundPointsSize(file);
      setGroundPoints(readGroundPoints(file, size));

      std::fclose(file);

    } catch (...) 	{
      TL_THROW_EXCEPTION_WITH_NESTED("");
    }

  }

  std::string format() const final override
  {
    return std::string("GRAPHOS");
  }

private:

  std::string readEpsg(FILE *file)
  {
    std::string code;

    try {

      uint32_t epsg = 0;
      std::fread(&epsg, sizeof(uint32_t), 1, file);

      if (epsg) {
        code.append("EPSG:");
        code.append(std::to_string(epsg));
        this->setEPSGCode(code);
      }

    } catch (...) {
      TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return code;
  }

  uint64_t readGroundPointsSize(FILE *file)
  {
    uint64_t size = 0;
    
    try {
      
      std::fread(&size, sizeof(uint64_t), 1, file);
    
    } catch (...) {
      TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return size;
  }

  std::vector<graphos::GroundPoint> readGroundPoints(FILE *file, uint64_t &size)
  {
    std::vector<graphos::GroundPoint> ground_points;

    try {

      for (auto &ground_point : ground_points) {

        std::fread(&ground_point.x, sizeof(double), 1, file);
        std::fread(&ground_point.y, sizeof(double), 1, file);
        std::fread(&ground_point.z, sizeof(double), 1, file);
        uint32_t color = 0;
        std::fread(&color, sizeof(uint32_t), 1, file);
        if (color)
          ground_point.setColor(tl::graph::Color(color));

        size = 0;
        std::fread(&size, sizeof(uint64_t), 1, file);
        Track track;

        size_t image_id = 0;
        size_t point_id = 0;

        for (size_t i = 0; i < size; i++) {

          std::fread(&image_id, sizeof(uint64_t), 1, file);
          std::fread(&size, sizeof(uint64_t), 1, file);

          track.addPair(image_id, point_id);

        }

        ground_point.setTrack(track);

      }

    } catch (...) {
      TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return ground_points;
  }
};


/* GroundPointsReaderFactory */

std::unique_ptr<GroundPointsReader> GroundPointsReaderFactory::create(const std::string &format)
{
  std::unique_ptr<GroundPointsReader> reader;

  try {

    if (format == "GRAPHOS") {
      reader = std::make_unique<GraphosGPsReader>();
    } else {
      TL_THROW_EXCEPTION("Invalid format: %s", format.c_str());
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

std::vector<GroundPoint> GroundPointsWriter::groundPoints() const
{
  return mGroundPoints;
}

std::string GroundPointsWriter::epsgCode() const
{
  return mEpsgCode;
}


class GraphosGPsWriter
  : public GroundPointsWriter
{
public:

  GraphosGPsWriter()
  {
  }

  ~GraphosGPsWriter()
  {
  }

// GroundPointsWriter

  void write(const tl::Path &path) override
  {
    try {

      FILE *file = std::fopen(path.toString().c_str(), "wb");

      TL_ASSERT(file, "File not open");

      uint32_t epsg = 0;
      std::string epsg_code = this->epsgCode();
      if (!epsg_code.empty()) {
        auto split_string = tl::split(epsg_code, ":");
        if (split_string.size() == 2) {
          epsg = std::stoi(split_string[1]);
        }
      }

      std::fwrite(&epsg, sizeof(uint32_t), 1, file);

      uint64_t size = this->groundPoints().size();
      std::fwrite(&size, sizeof(uint64_t), 1, file);

      for (auto &ground_point : this->groundPoints()) {

        std::fwrite(&ground_point.x, sizeof(double), 1, file);
        std::fwrite(&ground_point.y, sizeof(double), 1, file);
        std::fwrite(&ground_point.z, sizeof(double), 1, file);

        uint32_t color = ground_point.color();

        std::fwrite(&color, sizeof(uint32_t), 1, file);

        const auto &track = ground_point.track();
        size = track.size();
        std::fwrite(&size, sizeof(uint64_t), 1, file);

        for (const auto &pair : track.pairs()) {
          std::fwrite(&pair.first, sizeof(uint64_t), 1, file);
          std::fwrite(&pair.second, sizeof(uint64_t), 1, file);
        }

      }

      std::fclose(file);

    } catch (...) {
      TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }

  }

  std::string format() const final override
  {
    return std::string("GRAPHOS");
  }

};


/* GroundPointsWriterFactory */

std::unique_ptr<GroundPointsWriter> GroundPointsWriterFactory::create(const std::string &format)
{
  std::unique_ptr<GroundPointsWriter> writer;

  try {

    if (format == "GRAPHOS") {
      writer = std::make_unique<GraphosGPsWriter>();
    } else {
      TL_THROW_EXCEPTION("Invalid format: %s", format.c_str());
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

  return writer;
}

}
