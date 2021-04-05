#include "ImportCamerasModel.h"

#include "inspector/core/project.h"

#include <tidop/core/messages.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/math/algebra/euler_angles.h>
#include <tidop/math/algebra/quaternion.h>

#include <QStandardItemModel>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

namespace inspector
{

namespace ui
{

ImportCamerasModelImp::ImportCamerasModelImp(Project *project,
                                             QObject *parent)
  : ImportCamerasModel(parent),
    mProject(project),
    mCsvFile(""),
    bFieldNamesFromFirstRow(true),
    mDelimiter(","),
    mIniLine(0),
    mInputCrs(""),
    mItemModelCSV(new QStandardItemModel(this)),
    mItemModelCameras(new QStandardItemModel(this))
{
  this->init();
}

QStandardItemModel *ImportCamerasModelImp::itemModelCSV()
{
  return mItemModelCSV;
}

QStandardItemModel *ImportCamerasModelImp::itemModelFormatCameras()
{
  return mItemModelCameras;
}

void ImportCamerasModelImp::setCsvFile(const QString &csv)
{
  mCsvFile = csv;
}

void ImportCamerasModelImp::init()
{
}

void ImportCamerasModelImp::clear()
{
  mCsvFile.clear();
  bFieldNamesFromFirstRow = true;
  mDelimiter = ",";
  mIniLine = 0;
  mInputCrs = "";
  mItemModelCSV->clear();
  mItemModelCameras->clear();
}

void ImportCamerasModelImp::previewImportCameras()
{
  mItemModelCSV->clear();

  //emit parseOk(false); // Desactiva salvar

  QFile file(mCsvFile);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    if (mIniLine) {

      for (size_t i = 0; i < mIniLine; i++)
        stream.readLine();
    }

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

void ImportCamerasModelImp::previewImportCamerasFormated()
{
  bool checkX = false;
  bool checkY = false;
  bool checkZ = false;
  bool checkQw = false;
  bool checkQx = false;
  bool checkQy = false;
  bool checkQz = false;
  bool checkYaw = false;
  bool checkPitch = false;
  bool checkRoll = false;
  bool checkOmega = false;
  bool checkPhi = false;
  bool checkKappa = false;

  mItemModelCameras->clear();

  
  if (mRotationType.compare("Quaternions") == 0) {
    mItemModelCameras->setColumnCount(8);
    mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z" << "Qw" << "Qx" << "Qy" << "Qz");
  } else if (mRotationType.compare("Yaw, Pitch, Roll") == 0) {
    mItemModelCameras->setColumnCount(7);
    mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z" << "Yaw" << "Pitch" << "Roll");
  } else if (mRotationType.compare("Omega, Phi, Kappa") == 0) {
    mItemModelCameras->setColumnCount(7);
    mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z" << "Omega" << "Phi" << "Kappa");
  } else {
    mItemModelCameras->setColumnCount(4);
    mItemModelCameras->setHorizontalHeaderLabels(QStringList() << "Image" << "X" << "Y" << "Z");
  }

  QFile file(mCsvFile);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    if (mIniLine) {
      for (size_t i = 0; i < mIniLine; i++)
        stream.readLine();
    }

    QString line;
    if (bFieldNamesFromFirstRow){
      line = stream.readLine();
    }

    QString image;
    QString x;
    QString y;
    QString z;
    QString qx;
    QString qy;
    QString qz;
    QString qw;
    QString yaw;
    QString pitch;
    QString roll;
    QString omega;
    QString phi;
    QString kappa;

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

      if (mRotationType.compare("Quaternions") == 0) {

        it = mFieldIds.find("Qw");
        if (it != mFieldIds.end() && it->second != -1) {
          qw = reg.at(it->second);
          if (i == 0) qw.toDouble(&checkQw);
        } else {
          qw = "";
        }
        standardItem.append(new QStandardItem(qw));


        it = mFieldIds.find("Qx");
        if (it != mFieldIds.end() && it->second != -1) {
          qx = reg.at(it->second);
          if (i == 0) qx.toDouble(&checkQx);
        } else {
          qx = "";
        }
        standardItem.append(new QStandardItem(qx));


        it = mFieldIds.find("Qy");
        if (it != mFieldIds.end() && it->second != -1) {
          qy = reg.at(it->second);
          if (i == 0) qy.toDouble(&checkQy);
        } else {
          qy = "";
        }
        standardItem.append(new QStandardItem(qy));

            
        it = mFieldIds.find("Qz");
        if (it != mFieldIds.end() && it->second != -1) {
          qz = reg.at(it->second);
          if (i == 0) qz.toDouble(&checkQz);
        } else {
          qz = "";
        }
        standardItem.append(new QStandardItem(qz));

      } else if (mRotationType.compare("Yaw, Pitch, Roll") == 0) {

        it = mFieldIds.find("Yaw");
        if (it != mFieldIds.end() && it->second != -1) {
          yaw = reg.at(it->second);
          if (i == 0) yaw.toDouble(&checkYaw);
        } else {
          yaw = "";
        }
        standardItem.append(new QStandardItem(yaw));

        it = mFieldIds.find("Pitch");
        if (it != mFieldIds.end() && it->second != -1) {
          pitch = reg.at(it->second);
          if (i == 0) pitch.toDouble(&checkPitch);
        } else {
          pitch = "";
        }
        standardItem.append(new QStandardItem(pitch));

        it = mFieldIds.find("Roll");
        if (it != mFieldIds.end() && it->second != -1) {
          roll = reg.at(it->second);
          if (i == 0) roll.toDouble(&checkRoll);
        } else {
          roll = "";
        }
        standardItem.append(new QStandardItem(roll));

      } else if (mRotationType.compare("Omega, Phi, Kappa") == 0) {

        it = mFieldIds.find("Omega");
        if (it != mFieldIds.end() && it->second != -1) {
          omega = reg.at(it->second);
          if (i == 0) omega.toDouble(&checkOmega);
        } else {
          omega = "";
        }
        standardItem.append(new QStandardItem(omega));

        it = mFieldIds.find("Phi");
        if (it != mFieldIds.end() && it->second != -1) {
          phi = reg.at(it->second);
          if (i == 0) phi.toDouble(&checkPhi);
        } else {
          phi = "";
        }
        standardItem.append(new QStandardItem(phi));

        it = mFieldIds.find("Kappa");
        if (it != mFieldIds.end() && it->second != -1) {
          kappa = reg.at(it->second);
          if (i == 0) kappa.toDouble(&checkKappa);
        } else {
          kappa = "";
        }

        standardItem.append(new QStandardItem(kappa));
      }

      mItemModelCameras->insertRow(mItemModelCameras->rowCount(), standardItem);

      i++;
    }

    file.close();
  }

  
  if (mRotationType.compare("Quaternions") == 0) {
    emit parseOk(checkX && checkY && checkZ && checkQw && checkQx && checkQy && checkQz);
  } else if (mRotationType.compare("Yaw, Pitch, Roll") == 0) {
    emit parseOk(checkX && checkY && checkZ && checkYaw && checkPitch && checkRoll);
  } else if (mRotationType.compare("Omega, Phi, Kappa") == 0) {
    emit parseOk(checkX && checkY && checkZ && checkOmega && checkKappa && checkPhi);
  } else {
    emit parseOk(checkX && checkY && checkZ);
  }
}

void ImportCamerasModelImp::loadCameras()
{
  ///TODO: leer de proyecto

}

void ImportCamerasModelImp::setFieldNamesFromFirstRow(bool active)
{
  bFieldNamesFromFirstRow = active;
}

void ImportCamerasModelImp::setDelimiter(const QString &delimiter)
{
  mDelimiter = delimiter;
}

void ImportCamerasModelImp::setInitialLine(int iniLine)
{
  mIniLine = iniLine;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setImageFieldId(int id)
{
  mFieldIds["Image"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setXFieldId(int id)
{
  mFieldIds["X"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setYFieldId(int id)
{
  mFieldIds["Y"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setZFieldId(int id)
{
  mFieldIds["Z"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setRotationType(const QString &rotationType)
{
  mRotationType = rotationType;
}

void ImportCamerasModelImp::setQxFieldId(int id)
{
  mFieldIds["Qx"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setQyFieldId(int id)
{
  mFieldIds["Qy"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setQzFieldId(int id)
{
  mFieldIds["Qz"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setQwFieldId(int id)
{
  mFieldIds["Qw"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setYawFieldId(int id)
{
  mFieldIds["Yaw"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setPitchFieldId(int id)
{
  mFieldIds["Pitch"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setRollFieldId(int id)
{
  mFieldIds["Roll"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setOmegaFieldId(int id)
{
  mFieldIds["Omega"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setPhiFieldId(int id)
{
  mFieldIds["Phi"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setKappaFieldId(int id)
{
  mFieldIds["Kappa"] = id-1;
  previewImportCamerasFormated();
}

void ImportCamerasModelImp::setInputCRS(const QString &crs)
{
  mInputCrs = crs;
}

void ImportCamerasModelImp::setOutputCRS(const QString &crs)
{
  mOutputCrs = crs;
}

void ImportCamerasModelImp::importCameras()
{

  QFile file(mCsvFile);
  if (file.open(QFile::ReadOnly | QFile::Text)){
    QTextStream stream(&file);

    if (mIniLine) {
      for (size_t i = 0; i < mIniLine; i++)
        stream.readLine();
    }

    QString line;
    if (bFieldNamesFromFirstRow){
      line = stream.readLine();
    }

    QString image;
    QString x;
    QString y;
    QString z;
    QString qx;
    QString qy;
    QString qz;
    QString qw;
    QString yaw;
    QString pitch;
    QString roll;
    QString omega;
    QString phi;
    QString kappa;

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
      } else {
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

      if (mRotationType.compare("Quaternions") == 0) {

        it = mFieldIds.find("Qw");
        if (it != mFieldIds.end() && it->second != -1) {
          qw = reg.at(it->second);
        } else {
          qw = "";
        }

        it = mFieldIds.find("Qx");
        if (it != mFieldIds.end() && it->second != -1) {
          qx = reg.at(it->second);
        } else {
          qx = "";
        }

        it = mFieldIds.find("Qy");
        if (it != mFieldIds.end() && it->second != -1) {
          qy = reg.at(it->second);
        } else {
          qy = "";
        }
            
        it = mFieldIds.find("Qz");
        if (it != mFieldIds.end() && it->second != -1) {
          qz = reg.at(it->second);
        } else {
          qz = "";
        }

      } else if (mRotationType.compare("Yaw, Pitch, Roll") == 0) {

        it = mFieldIds.find("Yaw");
        if (it != mFieldIds.end() && it->second != -1) {
          yaw = reg.at(it->second);
        } else {
          yaw = "";
        }

        it = mFieldIds.find("Pitch");
        if (it != mFieldIds.end() && it->second != -1) {
          pitch = reg.at(it->second);
        } else {
          pitch = "";
        }

        it = mFieldIds.find("Roll");
        if (it != mFieldIds.end() && it->second != -1) {
          roll = reg.at(it->second);
        } else {
          roll = "";
        }

      } else if (mRotationType.compare("Omega, Phi, Kappa") == 0) {

        it = mFieldIds.find("Omega");
        if (it != mFieldIds.end() && it->second != -1) {
          omega = reg.at(it->second);
        } else {
          omega = "";
        }

        it = mFieldIds.find("Phi");
        if (it != mFieldIds.end() && it->second != -1) {
          phi = reg.at(it->second);
        } else {
          phi = "";
        }

        it = mFieldIds.find("Kappa");
        if (it != mFieldIds.end() && it->second != -1) {
          kappa = reg.at(it->second);
        } else {
          kappa = "";
        }
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

          tl::math::Quaternion<double> quaternion = tl::math::Quaternion<double>::identity();
          if (mRotationType.compare("Yaw, Pitch, Roll") == 0) {
            tl::math::EulerAngles<double> ypr(yaw.toDouble(), pitch.toDouble(), roll.toDouble(), tl::math::EulerAngles<double>::Axes::xyz);
            tl::math::RotationConverter<double>::convert(ypr, quaternion);
          } else if (mRotationType.compare("Omega, Phi, Kappa") == 0) {
            tl::math::EulerAngles<double> opk(omega.toDouble(), phi.toDouble(), kappa.toDouble(), tl::math::EulerAngles<double>::Axes::xyz);
            tl::math::RotationConverter<double>::convert(opk, quaternion);
          } else {
            quaternion.x = qx.toDouble();
            quaternion.y = qy.toDouble();
            quaternion.z = qz.toDouble();
            quaternion.w = qw.toDouble();
          }
          cameraPosition.setQuaternion(quaternion);

          image_it->setCameraPosition(cameraPosition);

          msgInfo("Camera coordinates found for %s : [%lf,%lf,%lf]", base_name.toStdString().c_str(), x.toDouble(), y.toDouble(), z.toDouble());
          break;
        }
      }

    }

    file.close();
  }

}

bool ImportCamerasModelImp::checkCRS(const QString &crs)
{
  tl::geospatial::Crs _crs(crs.toStdString());
  return _crs.isValid();
}

} // namespace ui

} // namespace inspector