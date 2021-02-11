#include "CameraPositionsModel.h"

#include "inspector/core/project.h"

#include <tidop/core/messages.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geometry/entities/point.h>

#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

namespace inspector
{

namespace ui
{

CamerasImportModelImp::CamerasImportModelImp(Project *project,
                                             QObject *parent)
  : CamerasImportModel(parent),
    mProject(project),
    mCsvFile(""),
    bFieldNamesFromFirstRow(true),
    mDelimiter(","),
    mInputCrs(""),
    mItemModelCSV(new QStandardItemModel(this)),
    mItemModelCameras(new QStandardItemModel(this))
{
  this->init();
}

QStandardItemModel *CamerasImportModelImp::itemModelCSV()
{
  return mItemModelCSV;
}

QStandardItemModel *CamerasImportModelImp::itemModelFormatCameras()
{
  return mItemModelCameras;
}

void CamerasImportModelImp::setCsvFile(const QString &csv)
{
  mCsvFile = csv;
}

void CamerasImportModelImp::init()
{
}

void CamerasImportModelImp::clear()
{
  mCsvFile.clear();
  bFieldNamesFromFirstRow = true;
  mDelimiter = ",";
  mInputCrs = "";
  mItemModelCSV->clear();
  mItemModelCameras->clear();
}

void CamerasImportModelImp::previewImportCameras()
{
  mItemModelCSV->clear();

  //emit parseOk(false); // Desactiva salvar

  QFile file(mCsvFile);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    QString line;
    if (bFieldNamesFromFirstRow){

      line = stream.readLine();
      QStringList header = line.split(mDelimiter);
      emit csvHeader(header);

      mItemModelCSV->setColumnCount(header.size());
      mItemModelCSV->setHorizontalHeaderLabels(header);

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
    }

    int i = 0;
    while (!stream.atEnd() && i < 20){

      line = stream.readLine();
      QStringList reg = line.split(mDelimiter);

      if (!bFieldNamesFromFirstRow && i == 0){

        QStringList header;
        for (int j = 0; j < reg.size(); j++){
          header.push_back(QString::number(j));
        }
        emit csvHeader(header);

        mItemModelCSV->setColumnCount(reg.size());
        mItemModelCSV->setHorizontalHeaderLabels(header);
      }


      QList<QStandardItem *> standardItem;
      for (const QString &item : reg){
        standardItem.append(new QStandardItem(item));
      }
      mItemModelCSV->insertRow(mItemModelCSV->rowCount(), 
                               standardItem);

      i++;
    }

    file.close();
  }
}

void CamerasImportModelImp::previewImportCamerasFormated()
{
  bool checkX = false;
  bool checkY = false;
  bool checkZ = false;

  mItemModelCameras->clear();

  mItemModelCameras->setColumnCount(4);
  mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z");

  QFile file(mCsvFile);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    QString line;
    if (bFieldNamesFromFirstRow){
      line = stream.readLine();
    }

    QString image;
    QString x;
    QString y;
    QString z;

    int i = 0;
    while (!stream.atEnd() && i < 20){

      line = stream.readLine();
      QStringList reg = line.split(mDelimiter);

      QList<QStandardItem *> standardItem;

      auto it = mFieldIds.find("Image");
      if (it != mFieldIds.end()) image = reg.at(it->second);
      else image = "";

      standardItem.append(new QStandardItem(image));

      it = mFieldIds.find("X");
      if (it != mFieldIds.end() && it->second != -1) {
        x = reg.at(it->second);
        if (i == 0) x.toDouble(&checkX);
      } else{
        x = "";
      }

      standardItem.append(new QStandardItem(x));

      it = mFieldIds.find("Y");
      if (it != mFieldIds.end() && it->second != -1) {
        y = reg.at(it->second);
        if (i == 0) y.toDouble(&checkY);
      } else {
        y = "";
      }

      standardItem.append(new QStandardItem(y));

      it = mFieldIds.find("Z");
      if (it != mFieldIds.end() && it->second != -1) {
        z = reg.at(it->second);
        if (i == 0) z.toDouble(&checkZ);
      } else {
        z = "";
      }

      standardItem.append(new QStandardItem(z));

      mItemModelCameras->insertRow(mItemModelCameras->rowCount(), standardItem);

      i++;
    }

    file.close();
  }

  emit parseOk(checkX && checkY && checkZ);

}

void CamerasImportModelImp::loadCameras()
{
  ///TODO: leer de proyecto

}

void CamerasImportModelImp::setFieldNamesFromFirstRow(bool active)
{
  bFieldNamesFromFirstRow = active;
}

void CamerasImportModelImp::setDelimiter(const QString &delimiter)
{
  mDelimiter = delimiter;
}

void CamerasImportModelImp::setImageFieldId(int id)
{
  mFieldIds["Image"] = id-1;
  previewImportCamerasFormated();
}

void CamerasImportModelImp::setXFieldId(int id)
{
  mFieldIds["X"] = id-1;
  previewImportCamerasFormated();
}

void CamerasImportModelImp::setYFieldId(int id)
{
  mFieldIds["Y"] = id-1;
  previewImportCamerasFormated();
}

void CamerasImportModelImp::setZFieldId(int id)
{
  mFieldIds["Z"] = id-1;
  previewImportCamerasFormated();
}

void CamerasImportModelImp::setInputCRS(const QString &crs)
{
  mInputCrs = crs;
}

void CamerasImportModelImp::setOutputCRS(const QString &crs)
{
  mOutputCrs = crs;
}

void CamerasImportModelImp::importCameras()
{

  QFile file(mCsvFile);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    QString line;
    if (bFieldNamesFromFirstRow){
      line = stream.readLine();
    }

    QString image;
    QString x;
    QString y;
    QString z;

    std::shared_ptr<tl::geospatial::Crs> crs_in(new tl::geospatial::Crs(mInputCrs.toStdString()));
    std::shared_ptr<tl::geospatial::Crs> crs_out(new tl::geospatial::Crs(mOutputCrs.toStdString()));
    bool bTrfCrs = crs_in->isValid() && crs_out->isValid();
    tl::geospatial::CrsTransform<tl::Point3D> crs_trf(crs_in, crs_out);

    while (!stream.atEnd()){

      line = stream.readLine();
      QStringList reg = line.split(mDelimiter);

      auto it = mFieldIds.find("Image");
      if (it != mFieldIds.end()) image = reg.at(it->second);
      else image = "";

      it = mFieldIds.find("X");
      if (it != mFieldIds.end() && it->second != -1) {
        x = reg.at(it->second);
      } else{
        x = "";
      }

      it = mFieldIds.find("Y");
      if (it != mFieldIds.end() && it->second != -1) {
        y = reg.at(it->second);
      } else {
        y = "";
      }

      it = mFieldIds.find("Z");
      if (it != mFieldIds.end() && it->second != -1) {
        z = reg.at(it->second);
      } else {
        z = "";
      }

      for (auto image_it = mProject->imageBegin(); image_it != mProject->imageEnd(); image_it++){
        QString base_name = QFileInfo(image).baseName();

        if (image_it->name().compare(base_name) == 0){

          CameraPosition cameraPosition;
          cameraPosition.setSource(mCsvFile);
          if (bTrfCrs){
            tl::Point3D pt_in;
            if (crs_in->isGeographic())
              pt_in = tl::Point3D(y.toDouble(), x.toDouble(), z.toDouble());
            else
              pt_in = tl::Point3D(x.toDouble(), y.toDouble(), z.toDouble());

            tl::Point3D pt_out = crs_trf.transform(pt_in);
            if (crs_out->isGeographic()) {
              cameraPosition.setX(pt_out.y);
              cameraPosition.setY(pt_out.x);
            } else {
              cameraPosition.setX(pt_out.x);
              cameraPosition.setY(pt_out.y);
            }
            cameraPosition.setZ(pt_out.z);
            cameraPosition.setCrs(mOutputCrs);
          } else {
            cameraPosition.setX(x.toDouble());
            cameraPosition.setY(y.toDouble());
            cameraPosition.setZ(z.toDouble());
            cameraPosition.setCrs(mInputCrs);
          }

          image_it->setCameraPosition(cameraPosition);

          msgInfo("Camera coordinates found for %s : [%lf,%lf,%lf]", base_name.toStdString().c_str(), x.toDouble(), y.toDouble(), z.toDouble());
          break;
        }
      }

    }

    file.close();
  }

}

bool CamerasImportModelImp::checkCRS(const QString &crs)
{
  tl::geospatial::Crs _crs(crs.toStdString());
  return _crs.isValid();
}

} // namespace ui

} // namespace inspector
