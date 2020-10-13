#include "GeoreferenceModel.h"

#include "inspector/core/project.h"

#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>

namespace inspector
{

namespace ui
{

GeoreferenceModelImp::GeoreferenceModelImp(Project *project,
                                           QObject *parent)
  : GeoreferenceModel(parent),
    mProject(project),
    mItemModel(new QStandardItemModel(this))
{
  this->init();
}

QStandardItemModel *GeoreferenceModelImp::itemModel()
{
  return mItemModel;
}

void GeoreferenceModelImp::init()
{
}

void GeoreferenceModelImp::clear()
{
  mItemModel->clear();
}

void GeoreferenceModelImp::importPositionsFromCSV(const QString &csv, const QString &split)
{
  QFile file(csv);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    QString line = stream.readLine();
    QStringList header = line.split(split);
    emit csvHeader(header);
    mItemModel->setColumnCount(header.size());
    mItemModel->setHorizontalHeaderLabels(header);

    for (auto &label : header){
      if (label.compare("Image", Qt::CaseInsensitive) == 0 ||
          label.compare("Image", Qt::CaseInsensitive) == 0 ||
          label.compare("Img", Qt::CaseInsensitive) == 0){
        emit imageColumn(label);
      } else if (label.compare("X", Qt::CaseInsensitive) == 0 ||
          label.compare("Longitude", Qt::CaseInsensitive) == 0 ||
          label.compare("Lon", Qt::CaseInsensitive) == 0){
        emit xColumn(label);
      } else if (label.compare("Y", Qt::CaseInsensitive) == 0 ||
                 label.compare("Latitude", Qt::CaseInsensitive) == 0 ||
                 label.compare("Lat", Qt::CaseInsensitive) == 0){
        emit yColumn(label);
      } else if (label.compare("Z", Qt::CaseInsensitive) == 0 ||
                 label.compare("Altitude", Qt::CaseInsensitive) == 0 ||
                 label.compare("Alt", Qt::CaseInsensitive) == 0){
        emit zColumn(label);
      } else if (label.compare("Quat.x", Qt::CaseInsensitive) == 0 ||
                 label.compare("Qx", Qt::CaseInsensitive) == 0){
        emit qxColumn(label);
      } else if (label.compare("Quat.y", Qt::CaseInsensitive) == 0 ||
                 label.compare("Qy", Qt::CaseInsensitive) == 0){
        emit qyColumn(label);
      } else if (label.compare("Quat.z", Qt::CaseInsensitive) == 0 ||
                 label.compare("Qz", Qt::CaseInsensitive) == 0){
        emit qzColumn(label);
      } else if (label.compare("Quat.w", Qt::CaseInsensitive) == 0 ||
                 label.compare("Qw", Qt::CaseInsensitive) == 0){
        emit qwColumn(label);
      }
    }

    while (!stream.atEnd()){

      line = stream.readLine();
      QStringList reg = line.split(split);

      QList<QStandardItem *> standardItem;
      for (const QString &item : reg){
        standardItem.append(new QStandardItem(item));
      }
      mItemModel->insertRow(mItemModel->rowCount(), standardItem);
    }

    file.close();
  }
}

void GeoreferenceModelImp::loadCameraPositions()
{
  ///TODO: leer de proyecto

}

} // namespace ui

} // namespace inspector


