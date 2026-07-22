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

#include "ImportCamerasModel.h"

#include "graphos/core/project.h"

#include <tidop/core/app/Message.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geometry/primitives/Point.h>
#include <tidop/math/math.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/math/algebra/euler_angles.h>
#include <tidop/math/algebra/rotations/Quaternion.h>

///TODO: Esto no deber�a estar aqui
#include <colmap/base/database.h>
///

#include <QStandardItemModel>
#include <QTextStream>
#include <QFileInfo>

namespace graphos
{

ImportCamerasModelImp::ImportCamerasModelImp(Project *project,
                                             QObject *parent)
  : ImportCamerasModel(parent),
    mProject(project),
    mCsvFile(""),
    bFieldNamesFromFirstRow(true),
    mDelimiter(","),
    mIniLine(0),
    mCrs(""),
    mItemModelCSV(new QStandardItemModel(this)),
    mItemModelCameras(new QStandardItemModel(this))
{
    ImportCamerasModelImp::init();
}

auto ImportCamerasModelImp::itemModelCSV() -> QStandardItemModel*
{
    return mItemModelCSV;
}

auto ImportCamerasModelImp::itemModelFormatCameras() -> QStandardItemModel*
{
    return mItemModelCameras;
}

void ImportCamerasModelImp::setCsvFile(const QString &csv)
{
    mCsvFile = csv;
}

void ImportCamerasModelImp::init()
{
    //QString crs = mProject->crs();
    //if (!crs.isEmpty()) {
    //    mOutputCrs = crs;
    //}
}

void ImportCamerasModelImp::clear()
{
    mCsvFile.clear();
    bFieldNamesFromFirstRow = true;
    mDelimiter = ",";
    mIniLine = 0;
    mCrs = "";
    mItemModelCSV->clear();
    mItemModelCameras->clear();
}

void ImportCamerasModelImp::previewImportCameras()
{
    mItemModelCSV->clear();

    QFile file(mCsvFile);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        if (mIniLine) {

            for (size_t i = 0; i < static_cast<size_t>(mIniLine); i++)
                stream.readLine();
        }

        QString line;
        if (bFieldNamesFromFirstRow) {

            line = stream.readLine();
            QStringList header = line.split(mDelimiter);
            emit csvHeader(header);

            mItemModelCSV->setColumnCount(header.size());
            mItemModelCSV->setHorizontalHeaderLabels(header);

            for (auto &label : header) {
                if (label.compare("Image", Qt::CaseInsensitive) == 0 ||
                    label.compare("Image", Qt::CaseInsensitive) == 0 ||
                    label.compare("Img", Qt::CaseInsensitive) == 0) {
                    emit imageColumn(label);
                } else if (label.compare("X", Qt::CaseInsensitive) == 0 ||
                           label.compare("Longitude", Qt::CaseInsensitive) == 0 ||
                           label.compare("Lon", Qt::CaseInsensitive) == 0) {
                    emit xColumn(label);
                } else if (label.compare("Y", Qt::CaseInsensitive) == 0 ||
                           label.compare("Latitude", Qt::CaseInsensitive) == 0 ||
                           label.compare("Lat", Qt::CaseInsensitive) == 0) {
                    emit yColumn(label);
                } else if (label.compare("Z", Qt::CaseInsensitive) == 0 ||
                           label.compare("Altitude", Qt::CaseInsensitive) == 0 ||
                           label.compare("Alt", Qt::CaseInsensitive) == 0) {
                    emit zColumn(label);
                } else if (label.compare("Quat.x", Qt::CaseInsensitive) == 0 ||
                           label.compare("Qx", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Quaternions";
                    emit qxColumn(label);
                } else if (label.compare("Quat.y", Qt::CaseInsensitive) == 0 ||
                           label.compare("Qy", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Quaternions";
                    emit qyColumn(label);
                } else if (label.compare("Quat.z", Qt::CaseInsensitive) == 0 ||
                           label.compare("Qz", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Quaternions";
                    emit qzColumn(label);
                } else if (label.compare("Quat.w", Qt::CaseInsensitive) == 0 ||
                           label.compare("Qw", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Quaternions";
                    emit qwColumn(label);
                } else if (label.compare("Omega", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Omega, Phi, Kappa";
                    emit omegaColumn(label);
                } else if (label.compare("Phi", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Omega, Phi, Kappa";
                    emit phiColumn(label);
                } else if (label.compare("Kappa", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Omega, Phi, Kappa";
                    emit kappaColumn(label);
                } else if (label.compare("Yaw", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Yaw, Pitch, Roll";
                    emit yawColumn(label);
                } else if (label.compare("Pitch", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Yaw, Pitch, Roll";
                    emit pitchColumn(label);
                } else if (label.compare("Roll", Qt::CaseInsensitive) == 0) {
                    mRotationType = "Yaw, Pitch, Roll";
                    emit rollColumn(label);
                }
            }
        }

        int i = 0;
        while (!stream.atEnd() && i < 20) {

            line = stream.readLine().trimmed();
            //QStringList reg = line.split(mDelimiter);
            QStringList reg;
            QStringList _reg = line.split('"');
            if (_reg.size() == 3) {
                reg = _reg.at(2).trimmed().split(mDelimiter);
                reg.insert(0, _reg.at(1));
            } else {
                reg = line.split(mDelimiter);
            }

            if (!bFieldNamesFromFirstRow && i == 0) {

                QStringList header;
                for (int j = 0; j < reg.size(); j++) {
                    header.push_back(QString::number(j));
                }
                emit csvHeader(header);

                mItemModelCSV->setColumnCount(reg.size());
                mItemModelCSV->setHorizontalHeaderLabels(header);
            }


            QList<QStandardItem *> standardItem;
            for (const QString &item : reg) {
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
    try {

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
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            if (mIniLine) {
                for (size_t i = 0; i < static_cast<size_t>(mIniLine); i++)
                    stream.readLine();
            }

            QString line;
            if (bFieldNamesFromFirstRow) {
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
            while (!stream.atEnd() && i < 20) {

                line = stream.readLine();
                
                QStringList reg;
                QStringList _reg = line.trimmed().split('"');
                if (_reg.size() == 3) {
                    //reg.append(_reg.at(1));
                    reg = _reg.at(2).trimmed().split(mDelimiter);
                    reg.insert(0, _reg.at(1));
                } else {
                    reg = line.split(mDelimiter);
                }
                
                if (reg.size() < 4 ) continue;

                QList<QStandardItem *> standardItem;

                auto it = mFieldIds.find("Image");
                if (it != mFieldIds.end() && it->second != -1) {
                    image = reg.at(it->second);
                } else {
                    image = "";
                }

                standardItem.append(new QStandardItem(image));

                it = mFieldIds.find("X");
                if (it != mFieldIds.end() && it->second != -1) {
                    x = reg.at(it->second);
                    if (i == 0) x.toDouble(&checkX);
                } else {
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

    } catch (std::exception &e) {
        tl::printException(e);
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
    mFieldIds["Image"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setXFieldId(int id)
{
    mFieldIds["X"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setYFieldId(int id)
{
    mFieldIds["Y"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setZFieldId(int id)
{
    mFieldIds["Z"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setRotationType(const QString &rotationType)
{
    mRotationType = rotationType;
}

void ImportCamerasModelImp::setQxFieldId(int id)
{
    mFieldIds["Qx"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setQyFieldId(int id)
{
    mFieldIds["Qy"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setQzFieldId(int id)
{
    mFieldIds["Qz"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setQwFieldId(int id)
{
    mFieldIds["Qw"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setYawFieldId(int id)
{
    mFieldIds["Yaw"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setPitchFieldId(int id)
{
    mFieldIds["Pitch"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setRollFieldId(int id)
{
    mFieldIds["Roll"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setOmegaFieldId(int id)
{
    mFieldIds["Omega"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setPhiFieldId(int id)
{
    mFieldIds["Phi"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setKappaFieldId(int id)
{
    mFieldIds["Kappa"] = id - 1;
    previewImportCamerasFormated();
}

void ImportCamerasModelImp::setCrs(const QString &crs)
{
    mCrs = crs;
}

void ImportCamerasModelImp::writePriorPoses()
{
    colmap::Database database(mProject->database().toUtf8());

    for (const auto &pair_image : mProject->images()) {

        Image image = pair_image.second;

        tl::Path image_path(image.path().toStdString());

        for (auto image_colmap : database.ReadAllImages()) {

            tl::Path image_path_colmap(image_colmap.Name());

            if (image_path.equivalent(image_path_colmap)) {
                //colmap::Image image_colmap = database.ReadImageWithName(image_path);
                image_colmap.TvecPrior(0) = image.cameraPose().position().x;
                image_colmap.TvecPrior(1) = image.cameraPose().position().y;
                image_colmap.TvecPrior(2) = image.cameraPose().position().z;
                image_colmap.QvecPrior(0) = image.cameraPose().quaternion().w;
                image_colmap.QvecPrior(1) = image.cameraPose().quaternion().x;
                image_colmap.QvecPrior(2) = image.cameraPose().quaternion().y;
                image_colmap.QvecPrior(3) = image.cameraPose().quaternion().z;
                database.UpdateImage(image_colmap);

                break;
            }
        }

    }
}

void ImportCamerasModelImp::importCameras()
{

    QFile file(mCsvFile);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        if (mIniLine) {
            for (size_t i = 0; i < static_cast<size_t>(mIniLine); i++)
                stream.readLine();
        }

        QString line;
        if (bFieldNamesFromFirstRow) {
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

        //QString epsg_out = mProject->crs();
        //std::shared_ptr<tl::Crs> crs_in(new tl::Crs(mInputCrs.toStdString()));
        //std::shared_ptr<tl::Crs> crs_out(new tl::Crs(mOutputCrs.toStdString()));
        //mProject->setCrs(mOutputCrs);

        //bool bTrfCrs = crs_in->isValid() && crs_out->isValid();

        while (!stream.atEnd()) {

            line = stream.readLine().trimmed();
            //QStringList reg = line.split(mDelimiter);
            QStringList reg;
            QStringList _reg = line.split('"');
            if (_reg.size() == 3) {
                reg = _reg.at(2).trimmed().split(mDelimiter);
                reg.insert(0, _reg.at(1));
            } else {
                reg = line.split(mDelimiter);
            }

            if (reg.size() < 4 ) continue;

            auto it = mFieldIds.find("Image");
            if (it != mFieldIds.end() && it->second != -1) {
                image = reg.at(it->second);
            } else {
                image = "";
            }

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

            for (const auto &pair_image : mProject->images()) {

                tl::Path image_path(image.toStdString());
                tl::Path image_path2(pair_image.second.path().toStdString());

                if (image_path.equivalent(image_path2)) {

                    CameraPose camera_pose;
                    camera_pose.setSource(mCsvFile);
                    //if (bTrfCrs) {
                    //    tl::CrsTransform crs_trf(crs_in, crs_out);

                    //    tl::Point3d pt_in;
                    //    pt_in = tl::Point3d(x.toDouble(), y.toDouble(), z.toDouble());

                    //    tl::Point3d pt_out = crs_trf.transform(pt_in);

                    //    camera_pose.setPosition(pt_out);
                    //    camera_pose.setCrs(mOutputCrs);
                    //} else {
                        camera_pose.setPosition(tl::Point3d(x.toDouble(), y.toDouble(), z.toDouble()));
                        camera_pose.setCrs(mCrs);
                        //TODO: Se tiene que pasar la precisión de los puntos
                        camera_pose.setAccuracy({0.01, 0.01, 0.01});
                        // Aqui se puede poner el estado RTK. Supongo que la precisión de las camaras es buena
                        camera_pose.setRtkFlag(50);
                    //}

                    tl::Quaternion<double> quaternion = tl::Quaternion<double>::identity();
                    if (mRotationType.compare("Yaw, Pitch, Roll") == 0) {
                        tl::EulerAngles<double> ypr(yaw.toDouble() * tl::consts::deg_to_rad<double>,
                                                    pitch.toDouble() * tl::consts::deg_to_rad<double>,
                                                    roll.toDouble() * tl::consts::deg_to_rad<double>);
                        tl::RotationConverter<double>::convert(ypr, quaternion);
                    } else if (mRotationType.compare("Omega, Phi, Kappa") == 0) {
                        tl::EulerAngles<double> opk(omega.toDouble() * tl::consts::deg_to_rad<double>,
                                                    phi.toDouble() * tl::consts::deg_to_rad<double>,
                                                    kappa.toDouble() * tl::consts::deg_to_rad<double>);
                        tl::RotationConverter<double>::convert(opk, quaternion);
                    } else {
                        quaternion.x = qx.toDouble();
                        quaternion.y = qy.toDouble();
                        quaternion.z = qz.toDouble();
                        quaternion.w = qw.toDouble();
                    }

                    quaternion.normalize();
                    camera_pose.setQuaternion(quaternion);

                    Image image = pair_image.second;
                    image.setCameraPose(camera_pose);
                    mProject->updateImage(pair_image.first, image);
                    tl::Message::info("Camera coordinates found for {} : [{},{},{}]", image.name().toStdString(), x.toDouble(), y.toDouble(), z.toDouble());
                    break;
                }
            }

        }

        file.close();
    }

    writePriorPoses();

}

struct ImageData
{
    int sequence;
    double seconds_of_week;
    int gps_week;
    double offset_north;
    double offset_east;
    double offset_vertical;
    double latitude;
    double longitude;
    double height;
    double std_dev_north;
    double std_dev_east;
    double std_dev_vertical;
    int rtk_status;
    std::string image_name;
};

void ImportCamerasModelImp::importCamerasFromMRK(const QString &file)
{
    try {

        std::ifstream stream(file.toStdString());
        TL_ASSERT(stream.is_open(), "Error when opening the file {}", file.toStdString());

        std::string line;
        while (std::getline(stream, line)) {

            auto columns = tl::split<std::string>(line, '\t');

            TL_ASSERT(columns.size() == 11, "");

            ImageData entry;
            entry.sequence = std::stoi(columns[0]);
            entry.seconds_of_week = std::stod(columns[1]);

            size_t pos1 = columns[2].find("[");
            size_t pos2 = columns[2].find("]");
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                entry.gps_week = std::stoi(columns[2].substr(pos1 + 1, pos2 - pos1 + 1));
            }

            tl::trim(columns[3]);
            auto offset_north = tl::split<std::string>(columns[3]);
            entry.offset_north = std::stod(offset_north.at(0));

            tl::trim(columns[4]);
            auto offset_east = tl::split<std::string>(columns[4]);
            entry.offset_east = std::stod(offset_east.at(0));

            tl::trim(columns[5]);
            auto offset_vertical = tl::split<std::string>(columns[5]);
            entry.offset_vertical = std::stod(offset_vertical.at(0));

            tl::trim(columns[6]);
            auto latitude = tl::split<std::string>(columns[6]);
            entry.latitude = std::stod(latitude.at(0));

            tl::trim(columns[7]);
            auto longitude = tl::split<std::string>(columns[7]);
            entry.longitude = std::stod(longitude.at(0));

            tl::trim(columns[8]);
            auto height = tl::split<std::string>(columns[8]);
            entry.height = std::stod(height.at(0));

            // Dividir las desviaciones estándar y el estado RTK
            auto std_dev = tl::split<std::string>(columns[9], ',');
            entry.std_dev_north = std::stod(tl::trim_copy(std_dev[0]));
            entry.std_dev_east = std::stod(tl::trim_copy(std_dev[1]));
            entry.std_dev_vertical = std::stod(tl::trim_copy(std_dev[2]));

            tl::trim(columns[10]);
            auto rtk_status = tl::split<std::string>(columns[10]);
            entry.rtk_status = std::stoi(rtk_status.at(0));

            std::ostringstream sequence_pattern;
            sequence_pattern << "_" << std::setw(4) << std::setfill('0') << entry.sequence;
            std::string pattern = sequence_pattern.str();

            for (const auto &image_pair : mProject->images()) {

                auto image = image_pair.second;
                auto name = image.name().toStdString();

                // Buscar si el índice aparece en el nombre del archivo
                if (name.find(pattern) != std::string::npos) {

                    tl::Point3d pt(entry.longitude, entry.latitude, entry.height);

                    CameraPose camera_pose;
                    camera_pose.setSource(file);
                    camera_pose.setPosition(pt);
                    camera_pose.setCrs("EPSG:4326");
                    camera_pose.setAccuracy({entry.std_dev_east, entry.std_dev_north, entry.std_dev_vertical});
                    camera_pose.setRtkFlag(entry.rtk_status);

                    image.setCameraPose(camera_pose);
                    mProject->updateImage(image_pair.first, image);
                    tl::Message::info("Camera coordinates found for {} : [{},{},{}]", image.name().toStdString(), pt.x, pt.y, pt.z);

                    break;
                }
            }
        }

        stream.close();

        writePriorPoses();

    } catch (...){
        TL_THROW_EXCEPTION_WITH_NESTED("Error reading mrk file");
    }
}


} // namespace graphos
