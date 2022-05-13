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

//tl::Point3<double> GroundControlPoint::point() const
//{
//  return mCoordinates;
//}

void GroundControlPoint::setPoint(const tl::Point3<double> &point)
{
  this->x = point.x;
  this->y = point.y;
  this->z = point.z;
}

//double GroundControlPoint::x() const
//{
//  return mCoordinates.x;
//}
//
//void GroundControlPoint::setX(double x)
//{
//  mCoordinates.x = x;
//}
//
//double GroundControlPoint::y() const
//{
//  return mCoordinates.y;
//}
//
//void GroundControlPoint::setY(double y)
//{
//  mCoordinates.y = y;
//}
//
//double GroundControlPoint::z() const
//{
//  return mCoordinates.z;
//}
//
//void GroundControlPoint::setZ(double z)
//{
//  mCoordinates.z = z;
//}

//void GroundControlPoint::addTrackPoint(size_t imageId, 
//                                       const tl::Point<double> &point)
//{
//  mTrack.addPoint(imageId, point);
//}
//
//tl::PointD GroundControlPoint::imagePoint(size_t imageId) const
//{
//  return mTrack.point(imageId);
//}
//
//bool GroundControlPoint::existImagePoint(size_t imageId) const
//{
//  auto it = mPoints.find(imageId);
//  
//  return (it != mPoints.end());
//}

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



std::vector<GroundControlPoint> groundControlPointsRead(const tl::Path &gcpFile)
{
  std::vector<GroundControlPoint> ground_control_points;

  QString gcp_file = QString::fromStdString(gcpFile.toString());
  QFile file(gcp_file);
  if(file.open(QFile::ReadOnly)) {

    QXmlStreamReader stream;
    stream.setDevice(&file);

    if(stream.readNextStartElement()) {
      if(stream.name() == "Graphos") {
        while(stream.readNextStartElement()) {
          if(stream.name() == "GroundControlPoints") {
            while(stream.readNextStartElement()) {
              if(stream.name() == "Crs") {
                ///TODO: Ahora no lo estoy usando
                QString crs = stream.readElementText();
              } else if(stream.name() == "GroundControlPoint") {

                GroundControlPoint gcp;
                GCPTrack track;

                while(stream.readNextStartElement()) {
                  if(stream.name() == "Name") {
                    gcp.setName(stream.readElementText().toStdString());
                  } else if(stream.name() == "x") {
                    gcp.x = stream.readElementText().toDouble();
                  } else if(stream.name() == "y") {
                    gcp.y = stream.readElementText().toDouble();
                  } else if(stream.name() == "z") {
                    gcp.z = stream.readElementText().toDouble();
                  } else if(stream.name() == "error") {
                    QString error = stream.readElementText();
                  } else if(stream.name() == "ImagePoints") {

                    while(stream.readNextStartElement()) {

                      if(stream.name() == "ImagePoint") {


                        size_t image_id = 0;
                        tl::PointD point_2d;

                        for (auto &attr : stream.attributes()) {
                          if (attr.name().compare(QString("image_id")) == 0) {
                            image_id = attr.value().toULongLong();
                            break;
                          }
                        }

                        while(stream.readNextStartElement()) {
                          if(stream.name() == "x") {
                            point_2d.x = stream.readElementText().toDouble();
                          } else if(stream.name() == "y") {
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
                ground_control_points.push_back(gcp);

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

  return ground_control_points;
}




void writeGroundControlPoints(QXmlStreamWriter &stream,
                              const std::vector<GroundControlPoint> &gcps)
{
  stream.writeStartElement("GroundControlPoints");
  {
    //stream.writeTextElement("Crs", mCrs);

    for (int i = 0; i < gcps.size(); i++) {

      auto gcp = gcps[i];

      stream.writeStartElement("GroundControlPoint");
      stream.writeTextElement("Name", QString::fromStdString(gcps[i].name()));
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
}

void groundControlPointsWrite(const tl::Path &gcpFile, 
                              const std::vector<GroundControlPoint> &gcps)
{
  QString gcp_file = QString::fromStdString(gcpFile.toString());
  QFile file(gcp_file);

  if (file.open(QFile::WriteOnly)) {

    QXmlStreamWriter stream(&file);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("Graphos");
    writeGroundControlPoints(stream, gcps);
    stream.writeEndElement();

    file.close();
  }

}


}
