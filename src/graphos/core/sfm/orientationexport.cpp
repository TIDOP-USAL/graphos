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

#include "graphos/core/sfm/orientationexport.h"

#include <colmap/base/reconstruction.h>
#include <colmap/util/bitmap.h>
#include <colmap/util/misc.h>
#include <colmap/base/camera_database.h>

#include <tidop/core/msg/message.h>
#include <tidop/math/algebra/rotation_matrix.h>

#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QApplication>

#ifdef _DEBUG
  /// Borrar
#include "colmap/base/projection.h"
#include "colmap/base/point3d.h"
#endif

using namespace Qt;

namespace graphos
{

OrientationExport::OrientationExport(const colmap::Reconstruction *reconstruction,
                                     const tl::Point3<double> &offset)
  : mReconstruction(reconstruction),
    mOffset(offset)
{
}

OrientationExport::~OrientationExport()
{
}

void OrientationExport::exportBinary(const QString &path) const
{
    if (mReconstruction)
        mReconstruction->WriteBinary(path.toStdString());
    else
        tl::Message::error("There is not a valid reconstruction");
}

void OrientationExport::exportText(const QString &path) const
{
    if (mReconstruction)
        mReconstruction->WriteText(path.toStdString());
    else
        tl::Message::error("There is not a valid reconstruction");
}

void OrientationExport::exportNVM(const QString &path) const
{
    if (mReconstruction)
        mReconstruction->ExportNVM(path.toStdString());
    else
        tl::Message::error("There is not a valid reconstruction");
}

struct PointPly
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float nx = 0.0f;
    float ny = 0.0f;
    float nz = 0.0f;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

/// Extraido de colmap para filtra los puntos para visualización

void writeBinaryPlyPoints(const std::string &path,
                          const std::vector<PointPly> &points,
                          bool write_normal, bool write_rgb)
{
    std::fstream text_file(path, std::ios::out);
    if (!text_file.is_open()) throw std::runtime_error(std::string("File not open: ").append(path));

    text_file << "ply\n";
    text_file << "format binary_little_endian 1.0\n";
    text_file << "element vertex " << points.size() << '\n';

    text_file << "property float x\n";
    text_file << "property float y\n";
    text_file << "property float z\n";

    if (write_normal) {
        text_file << "property float nx\n";
        text_file << "property float ny\n";
        text_file << "property float nz\n";
    }

    if (write_rgb) {
        text_file << "property uchar red\n";
        text_file << "property uchar green\n";
        text_file << "property uchar blue\n";
    }

    text_file << "end_header\n";
    text_file.close();

    std::fstream binary_file(path,
                             std::ios::out | std::ios::binary | std::ios::app);
    if (!binary_file.is_open()) throw std::runtime_error(std::string("File not open: ").append(path));

    for (const auto &point : points) {
        colmap::WriteBinaryLittleEndian<float>(&binary_file, point.x);
        colmap::WriteBinaryLittleEndian<float>(&binary_file, point.y);
        colmap::WriteBinaryLittleEndian<float>(&binary_file, point.z);

        if (write_normal) {
            colmap::WriteBinaryLittleEndian<float>(&binary_file, point.nx);
            colmap::WriteBinaryLittleEndian<float>(&binary_file, point.ny);
            colmap::WriteBinaryLittleEndian<float>(&binary_file, point.nz);
        }

        if (write_rgb) {
            colmap::WriteBinaryLittleEndian<uint8_t>(&binary_file, point.r);
            colmap::WriteBinaryLittleEndian<uint8_t>(&binary_file, point.g);
            colmap::WriteBinaryLittleEndian<uint8_t>(&binary_file, point.b);
        }
    }

    binary_file.close();
}

void OrientationExport::exportPLY(const QString &path) const
{
    if (mReconstruction) {

        std::vector<PointPly> ply_points;
        ply_points.reserve(mReconstruction->NumPoints3D());

        for (auto &point : mReconstruction->Points3D()) {
            if (point.second.Error() < 2 &&
                point.second.Track().Length() >= 3) {
                PointPly ply_point;
                ply_point.x = static_cast<float>(point.second.X());
                ply_point.y = static_cast<float>(point.second.Y());
                ply_point.z = static_cast<float>(point.second.Z());
                ply_point.r = point.second.Color(0);
                ply_point.g = point.second.Color(1);
                ply_point.b = point.second.Color(2);
                ply_points.push_back(ply_point);
            }
        }

        bool kWriteNormal = false;
        bool kWriteRGB = true;
        writeBinaryPlyPoints(path.toStdString(), ply_points, kWriteNormal, kWriteRGB);

    } else
        tl::Message::error("There is not a valid reconstruction");
}

void OrientationExport::exportVRML(const QString &path) const
{
    if (mReconstruction) {
        mReconstruction->ExportVRML(path.toStdString() + "/images.wrl",
                                    path.toStdString() + "/points3D.wrl", 1,
                                    Eigen::Vector3d(1, 0, 0));
    } else {
        tl::Message::error("There is not a valid reconstruction");
    }
}

void OrientationExport::exportOrientation(const QString &path, bool bQuaternion) const
{
    if (mReconstruction) {
        /// Exportación personalizada
        //QFile file(path);

        std::ofstream stream(path.toStdString(), std::ios::trunc);
        if (stream.is_open()) {
            //QTextStream stream(&file);

            if (bQuaternion) {
                stream << "# IMAGE_ID QW QX QY QZ TX TY TZ CAMERA_ID NAME" << std::endl;
            } else {
                stream << "imageName X Y Z Omega Phi Kappa" << std::endl;
            }

            for (auto &image : mReconstruction->Images()) {

                {

                    Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
                    Eigen::Vector3d translation = image.second.Tvec();
                    rotation_matrix(1, 0) = -rotation_matrix(1, 0);
                    rotation_matrix(1, 1) = -rotation_matrix(1, 1);
                    rotation_matrix(1, 2) = -rotation_matrix(1, 2);
                    rotation_matrix(2, 0) = -rotation_matrix(2, 0);
                    rotation_matrix(2, 1) = -rotation_matrix(2, 1);
                    rotation_matrix(2, 2) = -rotation_matrix(2, 2);

                    tl::RotationMatrix<double> _rotation_matrix;
                    _rotation_matrix.at(0, 0) = rotation_matrix(0, 0);
                    _rotation_matrix.at(0, 1) = rotation_matrix(0, 1);
                    _rotation_matrix.at(0, 2) = rotation_matrix(0, 2);
                    _rotation_matrix.at(1, 0) = rotation_matrix(1, 0);
                    _rotation_matrix.at(1, 1) = rotation_matrix(1, 1);
                    _rotation_matrix.at(1, 2) = rotation_matrix(1, 2);
                    _rotation_matrix.at(2, 0) = rotation_matrix(2, 0);
                    _rotation_matrix.at(2, 1) = rotation_matrix(2, 1);
                    _rotation_matrix.at(2, 2) = rotation_matrix(2, 2);

                    tl::RotationMatrix<double> rotation_transpose = _rotation_matrix.transpose();

                    tl::Point3<double> position;

                    position.x = -(rotation_transpose.at(0, 0) * translation[0] +
                                   rotation_transpose.at(0, 1) * -translation[1] +
                                   rotation_transpose.at(0, 2) * -translation[2]) + mOffset.x;
                    position.y = -(rotation_transpose.at(1, 0) * translation[0] +
                                   rotation_transpose.at(1, 1) * -translation[1] +
                                   rotation_transpose.at(1, 2) * -translation[2]) + mOffset.y;
                    position.z = -(rotation_transpose.at(2, 0) * translation[0] +
                                   rotation_transpose.at(2, 1) * -translation[1] +
                                   rotation_transpose.at(2, 2) * -translation[2]) + mOffset.z;

                    Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(0, 1, 2);

                    stream << std::fixed << "\"" << image.second.Name() << "\" " <<
                        position.x << " " <<
                        position.y << " " <<
                        position.z << " " <<
                        euler_angles[0] << " " <<
                        euler_angles[1] << " " <<
                        euler_angles[2] << " " << std::endl;
                }

                ////Eigen::Vector3d t = image.second.Tvec();
                //Eigen::Matrix<double, 3, 4> inv_proj_matrix = image.second.InverseProjectionMatrix();
                //Eigen::Vector3d t = inv_proj_matrix.rightCols<1>();
                //Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
                //rotation_matrix(1, 0) = -rotation_matrix(1, 0);
                //rotation_matrix(1, 1) = -rotation_matrix(1, 1);
                //rotation_matrix(1, 2) = -rotation_matrix(1, 2);
                //rotation_matrix(2, 0) = -rotation_matrix(2, 0);
                //rotation_matrix(2, 1) = -rotation_matrix(2, 1);
                //rotation_matrix(2, 2) = -rotation_matrix(2, 2);


                //if (bQuaternion){
                //  //Eigen::Vector4d quaternion = image.second.Qvec();
                //  Eigen::Quaterniond quaternion(rotation_matrix);
                //  stream << std::fixed << image.second.ImageId() << " " <<
                //            quaternion.w() << " " <<
                //            quaternion.x() << " " <<
                //            quaternion.y() << " " <<
                //            quaternion.z() << " " <<
                //            t[0] + mOffset.x << " " <<
                //            t[1] + mOffset.y << " " <<
                //            t[2] + mOffset.z << " " <<
                //            image.second.CameraId() << " " <<
                //            image.second.Name() << std::endl;
                //} else {
                //  
                //  Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(0, 1, 2);

                //  stream << std::fixed << "\"" << image.second.Name() << "\" " <<
                //            t[0] + mOffset.x << " " <<
                //            t[1] + mOffset.y << " " <<
                //            t[2] + mOffset.z << " " <<
                //            euler_angles[0] << " " <<
                //            euler_angles[1] << " " <<
                //            euler_angles[2] << " " << std::endl;
                //}

            }
            stream.close();
        }
    } else {
        tl::Message::error("There is not a valid reconstruction");
    }

}

void OrientationExport::exportPix4DCalibration(const QString &path, const QString &imagePath) const
{
    /// Se generan los archivos de calibración de cámaras de Pix4D

    /// Fichero de camara de Pix4D
    /// TODO: ver si hay varias camaras como queda ese fichero.
    /// El fichero tiene que llamarse: [nombre_proyecto]_calibrated_internal_camera_parameters.cam

    if (mReconstruction) {

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        QString database_cameras_path;
#ifdef _DEBUG
        database_cameras_path = QString(GRAPHOS_SOURCE_PATH).append("/res");
#else
        database_cameras_path = qApp->applicationDirPath();
#endif
        database_cameras_path.append("/cameras.db");
        db.setDatabaseName(database_cameras_path);

        if (db.open()) {

            for (auto &camera : mReconstruction->Cameras()) {

                double sensor_width_px;
                double sensor_width_mm = -1.0;
                double scale = 1.0;

                for (auto &image : mReconstruction->Images()) {
                    if (image.second.CameraId() == camera.second.CameraId()) {
                        std::string img_name = image.second.Name();
                        std::string img_file = std::string(imagePath.toStdString()).append("/").append(img_name);
                        colmap::Bitmap bmp;
                        if (bmp.Read(img_file, false)) {
                            sensor_width_px = std::max(bmp.Width(), bmp.Height());

                            // Lookup sensor width in database.
                            std::string camera_make;
                            std::string camera_model;
                            if (bmp.ReadExifTag(FIMD_EXIF_MAIN, "Make", &camera_make) &&
                                bmp.ReadExifTag(FIMD_EXIF_MAIN, "Model", &camera_model)) {

                                int id_camera = -1;
                                QSqlQuery query;
                                query.prepare("SELECT id_camera FROM cameras WHERE camera_make LIKE :camera_make LIMIT 1");
                                query.bindValue(":camera_make", camera_make.c_str());
                                if (query.exec()) {

                                    while (query.next()) {
                                        id_camera = query.value(0).toInt();
                                    }

                                } else {
                                    QSqlError err = query.lastError();
                                    tl::Message::error(err.text().toStdString());
                                    break;
                                }

                                if (id_camera == -1) {
                                    tl::Message::error("Camera '{}' not found in database", camera_make);
                                } else {
                                    query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
                                    query.bindValue(":camera_model", camera_model.c_str());
                                    query.bindValue(":id_camera", id_camera);
                                    if (query.exec()) {
                                        while (query.next()) {
                                            sensor_width_mm = query.value(0).toDouble();
                                            scale = sensor_width_mm / sensor_width_px;
                                            //msgInfo("Sensor size for camera %s %s is %f (mm)", camera_make.c_str(), camera_model.c_str(), sensor_width_mm);
                                        }

                                        if (sensor_width_mm < 0.) {
                                            tl::Message::error("Camera model ({} {}) not found in database", camera_make, camera_model);
                                        }

                                    } else {
                                        QSqlError err = query.lastError();
                                        tl::Message::error(err.text().toStdString());
                                    }
                                }
                                break;
                            }
                        }
                    }
                }

                QDir dir(path);
                std::string file_path = dir.filePath("calibrated_internal_camera_parameters.cam").toStdString();
                std::ofstream stream(file_path, std::ios::trunc);
                if (stream.is_open()) {

                    int model_id = camera.second.ModelId();
                    if (model_id == 0 || model_id == 2 || model_id == 3 || model_id == 50) {

                        size_t w = camera.second.Width();
                        size_t h = camera.second.Height();
                        //Eigen::Matrix3d calib_matrix = camera.second.CalibrationMatrix();
                        std::vector<double> params = camera.second.Params();

                        ///TODO: ¿El 0 se refiere a un identificador de cámara??
                        stream << "Pix4D camera calibration file 0\n";
                        stream << "#Focal Length mm assuming a sensor width of " <<
                            w * scale << "x" << h * scale << "mm" << '\n';
                        stream << "F " << camera.second.FocalLength() * scale << '\n';

                        stream << "#Principal Point mm" << '\n';
                        stream << "Px " << camera.second.PrincipalPointX() * scale << '\n';
                        stream << "Py " << camera.second.PrincipalPointY() * scale << '\n';

                        stream << "#Symmetrical Lens Distortion Coeffs" << '\n';
                        stream << "K1 " << (model_id == 0 ? 0.0 : params[3]) << '\n';
                        stream << "K2 " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << '\n';
                        stream << "K3 " << (model_id == 50 ? params[5] : 0.0) << '\n';

                        /// TODO: Controlar según el tipo de cámara.
                        stream << "#Tangential Lens Distortion Coeffs" << '\n';
                        stream << "T1 " << (model_id == 50 ? params[6] : 0.0) << '\n';
                        stream << "T2 " << (model_id == 50 ? params[7] : 0.0) << '\n' << '\n';
                    } else {
                        ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
                    }
                    stream.close();
                }


                file_path = dir.filePath("calibrated_internal_camera_parameters.cam").toStdString();
                stream.open(file_path, std::ios::trunc);
                if (stream.is_open()) {

                    int model_id = camera.second.ModelId();
                    if (model_id == 0 || model_id == 2 || model_id == 3 || model_id == 50) {

                        size_t w = camera.second.Width();
                        size_t h = camera.second.Height();
                        //Eigen::Matrix3d calib_matrix = camera.second.CalibrationMatrix();
                        std::vector<double> params = camera.second.Params();

                        double w_2_mm = w * scale / 2.;
                        double h_2_mm = h * scale / 2.;

                        ///TODO: ¿El 0 se refiere a un identificador de cámara??
                        stream << "camera_calibration_file 0\n";

                        stream << "#Focal Length mm assuming a sensor width of " <<
                            w * scale << "x" << h * scale << "mm" << '\n';
                        stream << "#Image size " << w << "x" << h << " pixel\n";
                        stream << "FOCAL " << camera.second.FocalLength() * scale << '\n' << '\n';

                        stream << "#Principal Point Offset xpoff ypoff in mm (Inpho)\n";
                        stream << "XPOFF " << (camera.second.PrincipalPointX() * scale) - w_2_mm << '\n';
                        stream << "YPOFF " << (camera.second.PrincipalPointY() * scale) - h_2_mm << '\n';
                        stream << "#Principal Point Offset xpoff ypoff in mm\n";
                        stream << "XPOFF " << w_2_mm - (camera.second.PrincipalPointX() * scale) << '\n';
                        stream << "YPOFF " << (camera.second.PrincipalPointY() * scale) - h_2_mm << '\n';
                        stream << "#Principal Point Offset xpoff ypoff in pixel\n";
                        stream << "XPOFF " << w / 2. - camera.second.PrincipalPointX() << '\n';
                        stream << "XPOFF " << camera.second.PrincipalPointY() - h / 2. << '\n' << '\n';

                        stream << "#How many fiducial pairs (max 8):\n";
                        stream << "NUM_FIDS 4\n\n";

                        stream << "#Fiducials position\n";
                        stream << "DATA_STRIP_SIDE left\n\n";

                        stream << "#Fiducial x,y pairs in mm:\n";
                        stream << "FID_PAIRS\n";
                        stream << "    " << w_2_mm << " " << -h_2_mm << '\n';
                        stream << "    " << -w_2_mm << " " << -h_2_mm << '\n';
                        stream << "    " << -w_2_mm << " " << h_2_mm << '\n';
                        stream << "    " << w_2_mm << " " << h_2_mm << '\n' << '\n';

                        /// La relación entre unos parametros y otros es:
                        /// K0' = 0;
                        /// K1' = K1 / f³
                        /// K2' = K2 / f⁵
                        /// K3' = K3 / f⁷
                        /// P1  = T1 / f²
                        /// P2  = T2 / f²
                        double f = camera.second.FocalLength() * scale;
                        stream << "#Symmetrical Lens Distortion Odd-order Poly Coeffs:K0,K1,K2,K3\n";
                        stream << "SYM_DIST 0 " <<
                            (model_id == 0 ? 0.0 : params[3] / pow(f, 3)) << " " <<
                            (model_id == 3 || model_id == 50 ? params[4] / pow(f, 5) : 0.0) << " " <<
                            (model_id == 50 ? params[5] / pow(f, 7) : 0.0) << '\n' << '\n';

                        stream << "#Decentering Lens Coeffs p1,p2,p3\n";
                        stream << "DEC_DIST " <<
                            (model_id == 50 ? params[6] / f * f : 0.0) << " " <<
                            (model_id == 50 ? params[7] / f * f : 0.0) << " 0" << '\n' << '\n';

                        stream << "#How many distortion pairs (max 20):\n";
                        stream << "NUM_DIST_PAIRS 20\n\n";

                    } else {
                        ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
                    }

                    stream.close();
                }

            }

        } else {
            tl::Message::error("Unable to establish a database connection");
            return;
        }

        db.close();

    } else {
        tl::Message::error("There is not a valid reconstruction");
    }

}

void OrientationExport::exportMVE(const QString &path) const
{

    if (mReconstruction) {

        QDir dir(path);
        if (!dir.exists()) {
            if (dir.mkpath(".") == false) {
                tl::Message::error("The output directory cannot be created: {}", path.toStdString());
                return;
            }
        }

        std::ofstream stream(dir.filePath("synth_0.out").toStdString(), std::ios::trunc);

        /// TODO: Si no puede abrir el fichero tiene que devolver un error
        if (stream.is_open()) {

            int camera_count = static_cast<int>(mReconstruction->Images().size());
            int feature_count = static_cast<int>(mReconstruction->NumPoints3D());

            stream << "drews 1.0\n";
            stream << camera_count << " " << feature_count << '\n';

            for (auto &camera : mReconstruction->Cameras()) {

                double sensor_width_px = static_cast<double>(std::max(camera.second.Width(), camera.second.Height()));
                double focal = camera.second.FocalLength() / sensor_width_px;
                double ppx = camera.second.PrincipalPointX() / sensor_width_px;
                double ppy = camera.second.PrincipalPointY() / sensor_width_px;
                int model_id = camera.second.ModelId();
                std::vector<double> params = camera.second.Params();

                //cv::Size imageSize;
                //cv::Mat cameraMatrix, distCoeffs;
                //cv::Mat map1, map2;
                //cv::Mat optCameraMat = getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0);
                //initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), optCameraMat, imageSize, CV_16SC2, map1, map2);

                TL_TODO("imagenes registradas mejor")
                for (auto &image : mReconstruction->Images()) {
                    if (image.second.CameraId() == camera.second.CameraId()) {

                        //std::string img_name = image.second.Name();

                        Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
                        Eigen::Vector3d translation = image.second.Tvec();

                        QString ini_file(path);
                        ini_file.append("\\views\\view_000").append(QString::number(image.second.ImageId() - 1)).append(".mve");
                        QDir dir_ini(ini_file);
                        if (!dir_ini.exists()) {
                            if (dir_ini.mkpath(".") == false) {
                                //msgWarning("The output directory cannot be created: %s", output_path.c_str());
                            }
                        }

                        std::ofstream stream_ini(QDir(ini_file).filePath("meta.ini").toStdString(), std::ios::trunc);

                        if (stream_ini.is_open()) {

                            stream_ini << "# MVE view meta data is stored in INI-file syntax.\n";
                            stream_ini << "# This file is generated, formatting will get lost.\n\n";
                            stream_ini << "[camera]\n";
                            stream_ini << "focal_length = " << focal << '\n';
                            stream_ini << "pixel_aspect = " << 1. << '\n';
                            //stream_ini << "principal_point = " << ppx << " " << ppy << endl;
                            stream_ini << "principal_point = 0.5 0.5\n";
                            stream_ini << "rotation = " << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << " "
                                << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << " "
                                << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << '\n';
                            stream_ini << "translation = " << translation[0] << " " << translation[1] << " " << translation[2] << '\n' << '\n';
                            stream_ini << "[view]\n";
                            stream_ini << "id = " << image.second.ImageId() - 1 << '\n';
                            stream_ini << "name = " << image.second.Name().c_str() << '\n';

                            stream_ini.close();
                        }

                        stream << focal << " " << (model_id == 0 ? 0 : params[3]) << " " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << '\n';
                        stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << '\n';
                        stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << '\n';
                        stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << '\n';
                        stream << translation[0] << " " << translation[1] << " " << translation[2] << '\n';

                    }
                }
            }


            for (auto &points_3d : mReconstruction->Points3D()) {

                Eigen::Vector3ub color = points_3d.second.Color();
                stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << '\n';

                //stream << 250 << " " << 100 << " " << 150 << endl;  // Write arbitrary RGB color, see above note
                stream << static_cast<int>(color[0]) << " " <<
                    static_cast<int>(color[1]) << " " <<
                    static_cast<int>(color[2]) << '\n';

                colmap::Track track = points_3d.second.Track();

                std::map<int, int> track_ids_not_repeat;
                for (auto &element : track.Elements()) {
                    track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
                }

                stream << track_ids_not_repeat.size();

                for (auto &map : track_ids_not_repeat) {
                    stream << " " << map.first << " " << map.second << " 0";
                }


                stream << '\n';
            }

            stream.close();

        }
    }
}

void OrientationExport::exportCMVS(const QString &path) const
{
    if (mReconstruction) {

        QDir dir(path);
        if (!dir.exists()) {
            if (dir.mkpath(".") == false) {
                tl::Message::error("The output directory cannot be created: {}", path.toStdString());
                return;
            }
        }


    }
}

void OrientationExport::exportBundler(const QString &oriFile) const
{

    if (mReconstruction) {

        std::ofstream stream(oriFile.toStdString(), std::ios::trunc);
        if (stream.is_open()) {

            size_t camera_count = mReconstruction->Images().size();
            size_t feature_count = mReconstruction->NumPoints3D();

            stream << "# Bundle file v0.3\n";
            stream << camera_count << " " << feature_count << '\n';

            for (auto &camera : mReconstruction->Cameras()) {

                double sensor_width_px = static_cast<double>(std::max(camera.second.Width(), camera.second.Height()));
                double focal = camera.second.FocalLength() / sensor_width_px;
                int model_id = camera.second.ModelId();
                std::vector<double> params = camera.second.Params();

                for (auto &image : mReconstruction->Images()) {

                    if (image.second.CameraId() == camera.second.CameraId()) {

                        std::string img_name = image.second.Name();

                        Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
                        Eigen::Vector3d translation = image.second.Tvec();
                        stream << focal << " " << (model_id == 0 ? 0 : params[3]) << " " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << '\n';
                        stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << '\n';
                        stream << -rotation_matrix(1, 0) << " " << -rotation_matrix(1, 1) << " " << -rotation_matrix(1, 2) << '\n';
                        stream << -rotation_matrix(2, 0) << " " << -rotation_matrix(2, 1) << " " << -rotation_matrix(2, 2) << '\n';
                        stream << translation[0] << " " << -translation[1] << " " << -translation[2] << '\n';

                    }
                }
            }


            for (auto &points_3d : mReconstruction->Points3D()) {

                Eigen::Vector3ub color = points_3d.second.Color();
                stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << '\n';

                stream << static_cast<int>(color[0]) << " " <<
                    static_cast<int>(color[1]) << " " <<
                    static_cast<int>(color[2]) << '\n';

                colmap::Track track = points_3d.second.Track();

                std::map<int, int> track_ids_not_repeat;
                for (auto &element : track.Elements()) {
                    track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
                }

                stream << track_ids_not_repeat.size();

                for (auto &map : track_ids_not_repeat) {

                    const colmap::Image &image = mReconstruction->Image(map.first + 1);
                    const colmap::Camera &camera = mReconstruction->Camera(image.CameraId());

                    const colmap::Point2D &point2D = image.Point2D(map.second);

                    stream << " " << map.first << " " << map.second << " ";
                    stream << point2D.X() - camera.PrincipalPointX() << " ";
                    stream << camera.PrincipalPointY() - point2D.Y() << " ";
                }

                stream << '\n';
            }
            stream.close();

        }
    } else
        tl::Message::error("There is not a valid reconstruction");
}

} // namespace graphos
