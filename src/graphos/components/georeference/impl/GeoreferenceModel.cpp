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

#include "GeoreferenceModel.h"

#include "graphos/core/project.h"
#include "graphos/core/sfm/groundpoint.h"

#include <QStandardItemModel>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/// TODO: Reemplazar la lectura escritura por groundControlPointsRead y groundControlPointsWrite

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
        ground_control_point.x = x;
        ground_control_point.y = y;
        ground_control_point.z = z;

        QList<QStandardItem *> items = mItemModelImagePoints->findItems(id, Qt::MatchExactly, 0);
        for (auto item : items) {
            int row = item->row();
            size_t image_id = mItemModelImagePoints->index(row, 1).data(Qt::UserRole + 1).toULongLong();
            double x = mItemModelImagePoints->index(row, 2).data().toDouble();
            double y = mItemModelImagePoints->index(row, 3).data().toDouble();
            ground_control_point.addPointToTrack(image_id, tl::Point<double>(x, y));
        }

        ground_control_points.push_back(ground_control_point);
    }

    return ground_control_points;
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
                size_t image_id = mItemModelImagePoints->index(row, 1).data(Qt::UserRole + 1).toULongLong();
                QString x = mItemModelImagePoints->index(row, 2).data().toString();
                QString y = mItemModelImagePoints->index(row, 3).data().toString();
                stream.writeAttribute("image_id", QString::number(image_id));
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
    ///TODO: Quitar esto y utilizar el m�todo de abajo
    mItemModelGroundControlPoints->clear();
    mItemModelImagePoints->clear();

    mItemModelGroundControlPoints->setColumnCount(5);
    QStringList header{"ID","X", "Y", "Z", "Error"};
    mItemModelGroundControlPoints->setHorizontalHeaderLabels(header);

    mItemModelImagePoints->setColumnCount(4);
    QStringList headerImagePoints{"CP", "Image", "X", "Y"};
    mItemModelImagePoints->setHorizontalHeaderLabels(headerImagePoints);

    tl::Path gcp_file = mProject->projectFolder();
    gcp_file.append("sfm").append("georef.xml");

    auto reader = GCPsReaderFactory::create("GRAPHOS");
    reader->read(gcp_file);
    //reader->epsgCode();
    auto gcps = reader->gcps();

    for (auto &gcp : gcps) {

        QString name = QString::fromStdString(gcp.name());
        QList<QStandardItem *> standardItem;
        standardItem.append(new QStandardItem(name));
        standardItem.append(new QStandardItem(QString::number(gcp.x, 'f', 3)));
        standardItem.append(new QStandardItem(QString::number(gcp.y, 'f', 3)));
        standardItem.append(new QStandardItem(QString::number(gcp.z, 'f', 3)));
        standardItem.append(new QStandardItem(QString()));

        for (auto &pair : gcp.track().points()) {

            size_t image_id = pair.first;
            tl::Point<double> point = pair.second;

            bool exist_image = mProject->existImage(image_id);
            if (!exist_image) continue;

            QString image = mProject->findImageById(image_id).name();            
            QString x = QString::number(point.x);
            QString y = QString::number(point.y);


            QList<QStandardItem *> standardItem;
            standardItem.append(new QStandardItem(name));
            standardItem.append(new QStandardItem(image));
            standardItem.append(new QStandardItem(x));
            standardItem.append(new QStandardItem(y));
            standardItem[1]->setData(static_cast<qulonglong>(image_id));
            mItemModelImagePoints->insertRow(mItemModelImagePoints->rowCount(),
                                             standardItem);
        }

        QStandardItem *item = standardItem.at(4);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        mItemModelGroundControlPoints->insertRow(mItemModelGroundControlPoints->rowCount(),
                                                 standardItem);

    }

}

void GeoreferenceModelImp::loadGroundControlPoints(const std::vector<GroundControlPoint> &gcps)
{
    mItemModelGroundControlPoints->clear();
    mItemModelImagePoints->clear();

    mItemModelGroundControlPoints->setColumnCount(5);
    QStringList header{"ID","X", "Y", "Z", "Error"};
    mItemModelGroundControlPoints->setHorizontalHeaderLabels(header);

    mItemModelImagePoints->setColumnCount(4);
    QStringList headerImagePoints{"CP", "Image", "X", "Y"};
    mItemModelImagePoints->setHorizontalHeaderLabels(headerImagePoints);

    tl::Path gcp_file = mProject->projectFolder();
    gcp_file.append("sfm").append("georef.xml");

    //auto reader = GCPsReaderFactory::create("GRAPHOS");
    //reader->read(gcp_file);
    ////reader->epsgCode();
    //auto gcps = reader->gcps();

    for (auto &gcp : gcps) {

        QString name = QString::fromStdString(gcp.name());
        QList<QStandardItem *> standardItem;
        standardItem.append(new QStandardItem(name));
        standardItem.append(new QStandardItem(QString::number(gcp.x, 'f', 3)));
        standardItem.append(new QStandardItem(QString::number(gcp.y, 'f', 3)));
        standardItem.append(new QStandardItem(QString::number(gcp.z, 'f', 3)));
        standardItem.append(new QStandardItem(QString()));

        for (auto &pair : gcp.track().points()) {

            size_t image_id = pair.first;
            tl::Point<double> point = pair.second;

            bool exist_image = mProject->existImage(image_id);
            if (!exist_image) continue;

            QString image = mProject->findImageById(image_id).name();            
            QString x = QString::number(point.x);
            QString y = QString::number(point.y);


            QList<QStandardItem *> standardItem;
            standardItem.append(new QStandardItem(name));
            standardItem.append(new QStandardItem(image));
            standardItem.append(new QStandardItem(x));
            standardItem.append(new QStandardItem(y));
            standardItem[1]->setData(static_cast<qulonglong>(image_id));
            mItemModelImagePoints->insertRow(mItemModelImagePoints->rowCount(),
                standardItem);
        }

        QStandardItem *item = standardItem.at(4);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        mItemModelGroundControlPoints->insertRow(mItemModelGroundControlPoints->rowCount(),
            standardItem);

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

const std::unordered_map<size_t, Image> &GeoreferenceModelImp::images() const
{
    return mProject->images();
}

Image GeoreferenceModelImp::image(size_t imageId) const
{
    return mProject->findImageById(imageId);
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
                                         size_t imageId,
                                         const QPointF &point)
{
    this->removeImagePoint(gcp, imageId);

    Image image = mProject->findImageById(imageId);

    QList<QStandardItem *> standardItem;
    standardItem.append(new QStandardItem(gcp));
    standardItem.append(new QStandardItem(image.name()));
    standardItem.append(new QStandardItem(QString::number(point.x())));
    standardItem.append(new QStandardItem(QString::number(point.y())));
    standardItem[1]->setData(static_cast<qulonglong>(imageId));
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

void GeoreferenceModelImp::removeImagePoint(const QString &gcp,
                                            size_t imageId)
{
    Image image = mProject->findImageById(imageId);

    QList<QStandardItem *> items = mItemModelImagePoints->findItems(image.name(), Qt::MatchExactly, 1);
    for (auto item : items) {
        int row = item->row();
        QString ground_control_point = mItemModelImagePoints->index(row, 0).data().toString();
        if (ground_control_point.compare(gcp) == 0) {
            mItemModelImagePoints->removeRow(row);
            break;
        }
    }
}

std::list<std::pair<QString, QPointF>> GeoreferenceModelImp::points(size_t imageId) const
{
    std::list<std::pair<QString, QPointF>> image_points;

    Image image = mProject->findImageById(imageId);

    QList<QStandardItem *> items = mItemModelImagePoints->findItems(image.name(), Qt::MatchExactly, 1);

    for (auto item : items) {
        int row = item->row();
        QString gcp = mItemModelImagePoints->index(row, 0).data().toString();
        QPointF point(mItemModelImagePoints->index(row, 2).data().toDouble(),
                      mItemModelImagePoints->index(row, 3).data().toDouble());
        image_points.push_back(std::make_pair(gcp, point));
    }

    return image_points;
}

tl::Path GeoreferenceModelImp::projectPath() const
{
    return mProject->projectFolder();
}

tl::Path GeoreferenceModelImp::reconstructionPath() const
{
    return mProject->reconstructionPath();
}

//void GeoreferenceModelImp::setReconstructionPath(const tl::Path &reconstructionPath)
//{
//    mProject->setReconstructionPath(reconstructionPath);
//}

void GeoreferenceModelImp::setSparseModel(const tl::Path &sparseModel)
{
    mProject->setSparseModel(sparseModel);
}

void GeoreferenceModelImp::setOffset(const tl::Path &offset)
{
    mProject->setOffset(offset);
}

void GeoreferenceModelImp::addPhotoOrientation(size_t imageId,
                                               const CameraPose &orientation)
{
    mProject->addPhotoOrientation(imageId, orientation);
}

const std::unordered_map<size_t, CameraPose> &GeoreferenceModelImp::poses() const
{
    return mProject->poses();
}

const std::map<int, Camera> &GeoreferenceModelImp::cameras() const
{
    return mProject->cameras();
}

std::vector<GroundPoint> GeoreferenceModelImp::groundPoints() const
{
    std::unique_ptr<GroundPointsReader> reader = GroundPointsReaderFactory::create("GRAPHOS");
    reader->read(mProject->groundPoints());

    return reader->points();
}

tl::Path GeoreferenceModelImp::database() const
{
    return mProject->database();
}

void GeoreferenceModelImp::setTransform(const tl::Matrix<double, 4, 4> &transform)
{
    mProject->setTransform(transform);
}

void GeoreferenceModelImp::setGroundPoints(const tl::Path &groundPoints)
{
    mProject->setGroundPoints(groundPoints);
}

bool GeoreferenceModelImp::updateCamera(int id, const Camera &camera)
{
    return mProject->updateCamera(id, camera);
}

void GeoreferenceModelImp::importGroundControlPoints(const QString &file, const QString &format)
{
    auto reader = GCPsReaderFactory::create(format.toStdString());
    reader->setImages(images());
    reader->read(file.toStdString());
    mCrs = QString::fromStdString(reader->epsgCode());
    auto gcps = reader->gcps();

    loadGroundControlPoints(gcps);
}

void GeoreferenceModelImp::exportGroundControlPoints(const QString &file, const QString &format)
{
    auto writer = GCPsWriterFactory::create(format.toStdString());
    writer->setEPSGCode(mCrs.toStdString());
    writer->setGCPs(groundControlPoints());
    writer->setImages(images());
    writer->write(file.toStdString());
}

void GeoreferenceModelImp::setCrs(const QString &crs)
{
    mCrs = crs;
}

void GeoreferenceModelImp::save()
{
    tl::Path gcp_file = mProject->projectFolder();
    gcp_file.append("sfm").append("georef.xml");

    QFile file(QString::fromStdWString(gcp_file.toWString()));

    if (file.open(QFile::WriteOnly)) {

        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();

        stream.writeStartElement("Graphos");
        writeGroundControlPoints(stream);
        stream.writeEndElement();

        file.close();
    }

    mProject->setCrs(mCrs);
}

} // namespace graphos
