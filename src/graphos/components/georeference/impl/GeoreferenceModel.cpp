/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "GeoreferenceModel.h"

#include "graphos/core/project.h"
#include "graphos/core/orientation/gcp.h"

#include <QStandardItemModel>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace graphos
{

GeoreferenceModelImp::GeoreferenceModelImp(Project *project,
                                           QObject *parent)
  : GeoreferenceModel(parent),
    mProject(project),
    mItemModelGroundControlPoints(new QStandardItemModel(this)),
    mItemModelImagePoints(new QStandardItemModel(this))
{
  this->init();
}

void GeoreferenceModelImp::init()
{
}

void GeoreferenceModelImp::clear()
{
  mItemModelGroundControlPoints->clear();
  mItemModelImagePoints->clear();
}

std::vector<GroundControlPoint> GeoreferenceModelImp::groundControlPoints() const
{
  std::vector<GroundControlPoint> ground_control_points;
      
  int rows = mItemModelGroundControlPoints->rowCount();

  for (int i = 0; i < rows; i++) {

    QString id = mItemModelGroundControlPoints->index(i, 0).data().toString();
    double x = mItemModelGroundControlPoints->index(i, 1).data().toDouble();
    double y = mItemModelGroundControlPoints->index(i, 2).data().toDouble();
    double z = mItemModelGroundControlPoints->index(i, 3).data().toDouble();
        
    GroundControlPoint ground_control_point;
    ground_control_point.setName(id.toStdString());
    ground_control_point.setX(x);
    ground_control_point.setY(y);
    ground_control_point.setZ(z);

    QList<QStandardItem *> items = mItemModelImagePoints->findItems(id, Qt::MatchExactly, 0);
    for (auto item : items) {
      int row = item->row();
      QString image = mItemModelImagePoints->index(row, 1).data().toString();
      double x = mItemModelImagePoints->index(row, 2).data().toDouble();
      double y = mItemModelImagePoints->index(row, 3).data().toDouble();
      ground_control_point.addImagePoint(image.toStdString(),tl::PointD(x, y));
    }

    ground_control_points.push_back(ground_control_point);
  }

  return ground_control_points;
}

void GeoreferenceModelImp::readGroundControlPoints(QXmlStreamReader &stream)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "Crs") {
      mCrs = stream.readElementText();
    } else if (stream.name() == "GroundControlPoint") {
      readGroundControlPoint(stream);
    } else
      stream.skipCurrentElement();
  }
}

void GeoreferenceModelImp::readGroundControlPoint(QXmlStreamReader &stream)
{
  QString gcp_id;
  QString x;
  QString y;
  QString z;
  QString error;

  while (stream.readNextStartElement()) {
    if (stream.name() == "Name") {
      gcp_id = stream.readElementText();
    } else if (stream.name() == "x") {
      x = stream.readElementText();
    } else if (stream.name() == "y") {
      y = stream.readElementText();
    } else if (stream.name() == "z") {
      z = stream.readElementText();
    } else if (stream.name() == "error") {
      error = stream.readElementText();
    } else if (stream.name() == "ImagePoints") {
      readImagePoints(stream, gcp_id);
    } else {
      stream.skipCurrentElement();
    }
  }

  QList<QStandardItem *> standardItem;
  standardItem.append(new QStandardItem(gcp_id));
  standardItem.append(new QStandardItem(x));
  standardItem.append(new QStandardItem(y));
  standardItem.append(new QStandardItem(z));
  standardItem.append(new QStandardItem(error));
  QStandardItem *item = standardItem.at(4);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  mItemModelGroundControlPoints->insertRow(mItemModelGroundControlPoints->rowCount(),
                                           standardItem);
}

void GeoreferenceModelImp::readImagePoints(QXmlStreamReader &stream, const QString &gcp_id)
{
  while (stream.readNextStartElement()) {
    if (stream.name() == "ImagePoint") {
      readImagePoint(stream, gcp_id);
    } else
      stream.skipCurrentElement();
  }
}

void GeoreferenceModelImp::readImagePoint(QXmlStreamReader &stream, const QString &gcp_id)
{
  QString image;
  QString x;
  QString y;

  while (stream.readNextStartElement()) {
    if (stream.name() == "Image") {
      image = stream.readElementText();
    } else if (stream.name() == "x") {
      x = stream.readElementText();
    } else if (stream.name() == "y") {
      y = stream.readElementText();
    } else {
      stream.skipCurrentElement();
    }
  }

  QList<QStandardItem *> standardItem;
  standardItem.append(new QStandardItem(gcp_id));
  standardItem.append(new QStandardItem(image));
  standardItem.append(new QStandardItem(x));
  standardItem.append(new QStandardItem(y));
  mItemModelImagePoints->insertRow(mItemModelImagePoints->rowCount(),
                                   standardItem);
}

void GeoreferenceModelImp::writeGroundControlPoints(QXmlStreamWriter &stream)
{
  stream.writeStartElement("GroundControlPoints");
  {
    stream.writeTextElement("Crs", mCrs);
    int rows = mItemModelGroundControlPoints->rowCount();

    for (int i = 0; i < rows; i++) {
      stream.writeStartElement("GroundControlPoint");
      QString id = mItemModelGroundControlPoints->index(i, 0).data().toString();
      QString x = mItemModelGroundControlPoints->index(i, 1).data().toString();
      QString y = mItemModelGroundControlPoints->index(i, 2).data().toString();
      QString z = mItemModelGroundControlPoints->index(i, 3).data().toString();
      QString error = mItemModelGroundControlPoints->index(i, 4).data().toString();
      stream.writeTextElement("Name", id);
      stream.writeTextElement("x", x);
      stream.writeTextElement("y", y);
      stream.writeTextElement("z", z);
      stream.writeTextElement("error", error);
      stream.writeStartElement("ImagePoints");
      QList<QStandardItem *> items = mItemModelImagePoints->findItems(id, Qt::MatchExactly, 0);
      for (auto item : items) {
        stream.writeStartElement("ImagePoint");
        int row = item->row();
        QString image = mItemModelImagePoints->index(row, 1).data().toString();
        QString x = mItemModelImagePoints->index(row, 2).data().toString();
        QString y = mItemModelImagePoints->index(row, 3).data().toString();
        stream.writeTextElement("Image", image);
        stream.writeTextElement("x", x);
        stream.writeTextElement("y", y);
        stream.writeEndElement();
      }
      stream.writeEndElement();
      stream.writeEndElement();
    }
  }
  stream.writeEndElement();
}

void GeoreferenceModelImp::loadGroundControlPoints()
{
  mItemModelGroundControlPoints->clear();
  mItemModelImagePoints->clear();

  mItemModelGroundControlPoints->setColumnCount(5);
  QStringList header{"ID","X", "Y", "Z", "Error"};
  mItemModelGroundControlPoints->setHorizontalHeaderLabels(header);

  mItemModelImagePoints->setColumnCount(4);
  QStringList headerImagePoints{"CP", "Image", "X", "Y"};
  mItemModelImagePoints->setHorizontalHeaderLabels(headerImagePoints);

  //QString gcp_file = mProject->reconstructionPath();
  QString gcp_file = mProject->projectFolder() + "\\ori\\relative";
  gcp_file.append("\\georef.xml");
  QFile file(gcp_file);
  if (file.open(QFile::ReadOnly)) {
    QXmlStreamReader stream;
    stream.setDevice(&file);

    if (stream.readNextStartElement()) {
      if (stream.name() == "Graphos") {
        while (stream.readNextStartElement()) {
          if (stream.name() == "GroundControlPoints") {
            this->readGroundControlPoints(stream);
          } else
            stream.skipCurrentElement();
        }
      } else {
        stream.raiseError(QObject::tr("Incorrect project file"));
      }

      file.close();
    }
  }
}

QString GeoreferenceModelImp::crs() const
{
  return mCrs;
}

QStandardItemModel *GeoreferenceModelImp::itemModelGroundControlPoints()
{
  return mItemModelGroundControlPoints;
}

QStandardItemModel *GeoreferenceModelImp::itemModelImagePoints()
{
  return mItemModelImagePoints;
}

std::vector<QString> GeoreferenceModelImp::images() const
{
  std::vector<QString> images;
  for (auto it = mProject->imageBegin(); it != mProject->imageEnd(); it++){
    images.push_back((*it).path());
  }
  return images;
}

void GeoreferenceModelImp::addGroundControlPoint()
{
  QList<QStandardItem *> standardItem;
  standardItem.append(new QStandardItem(""));
  standardItem.append(new QStandardItem(""));
  standardItem.append(new QStandardItem(""));
  standardItem.append(new QStandardItem(""));
  standardItem.append(new QStandardItem(""));
  QStandardItem *item = standardItem.at(4);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  mItemModelGroundControlPoints->insertRow(mItemModelGroundControlPoints->rowCount(),
                                           standardItem);
}

void GeoreferenceModelImp::removeGroundControlPoint(int index)
{
  mItemModelGroundControlPoints->removeRow(index);
}

void GeoreferenceModelImp::addImagePoint(const QString &gcp, 
                                         const QString &image, 
                                         const QPointF &point)
{
  this->removeImagePoint(gcp, image);

  QList<QStandardItem *> standardItem;
  standardItem.append(new QStandardItem(gcp));
  standardItem.append(new QStandardItem(image));
  standardItem.append(new QStandardItem(QString::number(point.x())));
  standardItem.append(new QStandardItem(QString::number(point.y())));
  QStandardItem *item = standardItem.at(0);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item = standardItem.at(1);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item = standardItem.at(2);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item = standardItem.at(3);
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);

  mItemModelImagePoints->insertRow(mItemModelImagePoints->rowCount(),
                                   standardItem);
}

void GeoreferenceModelImp::removeImagePoint(const QString &gcp, const QString &image)
{
  QList<QStandardItem *> items = mItemModelImagePoints->findItems(image, Qt::MatchExactly, 1);
  for (auto item : items) {
    int row = item->row();
    QString ground_control_point = mItemModelImagePoints->index(row, 0).data().toString();
    if (ground_control_point.compare(gcp) == 0) {
      mItemModelImagePoints->removeRow(row);
      break;
    }
  }
}

std::list<std::pair<QString, QPointF>> GeoreferenceModelImp::points(const QString &image) const
{
  std::list<std::pair<QString, QPointF>> image_points;

  QList<QStandardItem *> items = mItemModelImagePoints->findItems(image, Qt::MatchExactly, 1);
  for (auto item : items) {
    int row = item->row();
    QString gcp = mItemModelImagePoints->index(row, 0).data().toString();
    QPointF point(mItemModelImagePoints->index(row, 2).data().toDouble(),
                  mItemModelImagePoints->index(row, 3).data().toDouble());
    image_points.push_back(std::make_pair(gcp, point));
  }
  return image_points;
}

QString GeoreferenceModelImp::projectPath() const
{
  return mProject->projectFolder();
}

void GeoreferenceModelImp::setReconstructionPath(const QString &reconstructionPath)
{
  mProject->setReconstructionPath(reconstructionPath);
}

void GeoreferenceModelImp::setSparseModel(const QString &sparseModel)
{
  mProject->setSparseModel(sparseModel);
}

void GeoreferenceModelImp::setOffset(const QString &offset)
{
  mProject->setOffset(offset);
}

void GeoreferenceModelImp::addPhotoOrientation(const QString &imgName, 
                                               const CameraPose &orientation)
{
  mProject->addPhotoOrientation(imgName, orientation);
}

GeoreferenceModel::image_iterator GeoreferenceModelImp::imageBegin()
{
  return mProject->imageBegin();
}

GeoreferenceModel::image_const_iterator GeoreferenceModelImp::imageBegin() const
{
  return mProject->imageBegin();
}

GeoreferenceModel::image_iterator GeoreferenceModelImp::imageEnd()
{
  return mProject->imageEnd();
}

GeoreferenceModel::image_const_iterator GeoreferenceModelImp::imageEnd() const
{
  return mProject->imageEnd();
}

void GeoreferenceModelImp::setCrs(const QString &crs)
{
  mCrs = crs;
}

void GeoreferenceModelImp::save()
{
  //QString gcp_file = mProject->reconstructionPath();
  QString gcp_file = mProject->projectFolder() + "\\ori\\relative";
  gcp_file.append("\\georef.xml");
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
}

} // namespace graphos


