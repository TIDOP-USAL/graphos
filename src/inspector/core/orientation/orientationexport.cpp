#include "orientationexport.h"

#include <colmap/base/reconstruction.h>
#include <colmap/util/bitmap.h>
#include <colmap/util/misc.h>
#include <colmap/base/camera_database.h>

#include <tidop/core/messages.h>

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

using namespace Qt;

namespace inspector
{

OrientationExport::OrientationExport(const colmap::Reconstruction *reconstruction)
  : mReconstruction(reconstruction)
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
    msgError("There is not a valid reconstruction");
}

void OrientationExport::exportText(const QString &path) const
{
  if (mReconstruction)
    mReconstruction->WriteText(path.toStdString());
  else
    msgError("There is not a valid reconstruction");
}

void OrientationExport::exportNVM(const QString &path) const
{
  if (mReconstruction)
    mReconstruction->ExportNVM(path.toStdString());
  else
    msgError("There is not a valid reconstruction");
}

void OrientationExport::exportPLY(const QString &path) const
{
  if (mReconstruction)
    mReconstruction->ExportPLY(path.toStdString());
  else
    msgError("There is not a valid reconstruction");
}

void OrientationExport::exportVRML(const QString &path) const
{
  if (mReconstruction){
    mReconstruction->ExportVRML(path.toStdString() + "/images.wrl",
                                path.toStdString() + "/points3D.wrl", 1,
                                Eigen::Vector3d(1, 0, 0));
  } else {
    msgError("There is not a valid reconstruction");
  }
}

void OrientationExport::exportRelativeOrientation(const QString &path, bool bQuaternion) const
{
  if (mReconstruction){
    /// Exportación personalizada
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)){
      QTextStream stream(&file);

      if (bQuaternion){
        stream << "# IMAGE_ID QW QX QY QZ TX TY TZ CAMERA_ID NAME" << endl;
      } else{
        stream << "imageName X Y Z Omega Phi Kappa" << endl;
      }

      for(auto &image : mReconstruction->Images()){

        Eigen::Vector3d t = image.second.Tvec();

        if (bQuaternion){
          Eigen::Vector4d quaternion = image.second.Qvec();
          stream << image.second.ImageId() << " " <<
                    quaternion[0] << " " <<
                    quaternion[1] << " " <<
                    quaternion[2] << " " <<
                    quaternion[3] << " " <<
                    t[0] << " " <<
                    t[1] << " " <<
                    t[2] << " " <<
                    image.second.CameraId() << " " <<
                    image.second.Name().c_str() << endl;
        } else {
          Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
          Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(0, 1, 2);

          stream << image.second.Name().c_str() << " " <<
                    t[0] << " " <<
                    t[1] << " " <<
                    t[2] << " " <<
                    euler_angles[0] << " " <<
                    euler_angles[1] << " " <<
                    euler_angles[2] << " " << endl;
        }

      }
      file.close();
    }
  } else {
    msgError("There is not a valid reconstruction");
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
#ifdef _DEBUG
    db.setDatabaseName("/home/esteban/desarrollo/geomni/resources/cameras.db");
#else
    db.setDatabaseName("cameras.db");
#endif
    if(db.open()){

      for(auto &camera : mReconstruction->Cameras()){

        double sensor_width_px;
        double sensor_width_mm = -1.0;
        double scale = 1.0;

        for(auto &image : mReconstruction->Images()){
          if (image.second.CameraId() == camera.second.CameraId()){
            std::string img_name = image.second.Name();
            std::string img_file = std::string(imagePath.toStdString()).append("/").append(img_name);
            colmap::Bitmap bmp;
            if(bmp.Read(img_file, false)){
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
                if (query.exec()){

                  while(query.next()){
                    id_camera = query.value(0).toInt();
                  }

                } else {
                  QSqlError err = query.lastError();
                  msgError("%s", err.text().toStdString().c_str());
                  break;
                }

                if (id_camera == -1) {
                  msgError("Camera '%s' not found in database", camera_make.c_str());
                } else {
                  query.prepare("SELECT sensor_width FROM models WHERE camera_model LIKE :camera_model AND id_camera LIKE :id_camera");
                  query.bindValue(":camera_model", camera_model.c_str());
                  query.bindValue(":id_camera", id_camera);
                  if (query.exec()){
                    while(query.next()){
                      sensor_width_mm = query.value(0).toDouble();
                      scale = sensor_width_mm / sensor_width_px;
                      //msgInfo("Sensor size for camera %s %s is %f (mm)", camera_make.c_str(), camera_model.c_str(), sensor_width_mm);
                    }

                    if (sensor_width_mm < 0.){
                      msgError("Camera model (%s %s) not found in database", camera_make.c_str(), camera_model.c_str());
                    }

                  } else {
                    QSqlError err = query.lastError();
                    msgError("%s", err.text().toStdString().c_str());
                  }
                }
                break;
              }
            }
          }
        }

        QDir dir(path);
        TL_TODO("cambiar el nombre del archivo")
        QFile file2(dir.filePath("geomni_pix4d_calibrated_internal_camera_parameters.cam"));

        if (file2.open(QIODevice::WriteOnly)){
          QTextStream stream(&file2);

          int model_id = camera.second.ModelId();
          if (model_id == 0 || model_id == 2 || model_id == 3 || model_id == 50){

            size_t w = camera.second.Width();
            size_t h = camera.second.Height();
            //Eigen::Matrix3d calib_matrix = camera.second.CalibrationMatrix();
            std::vector<double> params = camera.second.Params();

            ///TODO: ¿El 0 se refiere a un identificador de cámara??
            stream << "Pix4D camera calibration file 0" << endl;
            stream << "#Focal Length mm assuming a sensor width of " <<
                      w * scale << "x" << h * scale << "mm" << endl;
            stream << "F " << camera.second.FocalLength() * scale << endl;

            stream << "#Principal Point mm" << endl;
            stream << "Px " << camera.second.PrincipalPointX()*scale << endl;
            stream << "Py " << camera.second.PrincipalPointY()*scale << endl;

            stream << "#Symmetrical Lens Distortion Coeffs" << endl;
            stream << "K1 " << (model_id == 0 ? 0.0 : params[3]) << endl;
            stream << "K2 " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << endl;
            stream << "K3 " << (model_id == 50 ? params[5] : 0.0) << endl;

            /// TODO: Controlar según el tipo de cámara.
            stream << "#Tangential Lens Distortion Coeffs" << endl;
            stream << "T1 " << (model_id == 50 ? params[6] : 0.0)  << endl;
            stream << "T2 " << (model_id == 50 ? params[7] : 0.0)  << endl << endl;
          } else {
            ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
          }
        }

        file2.close();

        QFile file3(dir.filePath("geomni_calibrated_internal_camera_parameters.cam"));
        if (file3.open(QIODevice::WriteOnly)){
          QTextStream stream(&file3);

          int model_id = camera.second.ModelId();
          if (model_id == 0 || model_id == 2 || model_id == 3 || model_id == 50){

            size_t w = camera.second.Width();
            size_t h = camera.second.Height();
            //Eigen::Matrix3d calib_matrix = camera.second.CalibrationMatrix();
            std::vector<double> params = camera.second.Params();

            double w_2_mm = w * scale / 2.;
            double h_2_mm = h * scale / 2.;

            ///TODO: ¿El 0 se refiere a un identificador de cámara??
            stream << "camera_calibration_file 0" << endl;

            stream << "#Focal Length mm assuming a sensor width of " <<
                        w * scale << "x" << h * scale << "mm" << endl;
            stream << "#Image size " << w << "x" << h << " pixel" << endl;
            stream << "FOCAL " << camera.second.FocalLength() * scale << endl << endl;

            stream << "#Principal Point Offset xpoff ypoff in mm (Inpho)" << endl;
            stream << "XPOFF " << (camera.second.PrincipalPointX() * scale) - w_2_mm << endl;
            stream << "YPOFF " << (camera.second.PrincipalPointY() * scale) - h_2_mm << endl;
            stream << "#Principal Point Offset xpoff ypoff in mm" << endl;
            stream << "XPOFF " << w_2_mm - (camera.second.PrincipalPointX() * scale) << endl;
            stream << "YPOFF " << (camera.second.PrincipalPointY() * scale) - h_2_mm << endl;
            stream << "#Principal Point Offset xpoff ypoff in pixel" << endl;
            stream << "XPOFF " << w/2. - camera.second.PrincipalPointX() << endl;
            stream << "XPOFF " << camera.second.PrincipalPointY() - h/2. << endl  << endl;

            stream << "#How many fiducial pairs (max 8):" << endl;
            stream << "NUM_FIDS 4" << endl << endl;

            stream << "#Fiducials position" << endl;
            stream << "DATA_STRIP_SIDE left" << endl << endl;

            stream << "#Fiducial x,y pairs in mm:" << endl;
            stream << "FID_PAIRS" << endl;
            stream << "    " << w_2_mm << " " << -h_2_mm << endl;
            stream << "    " << -w_2_mm << " " << -h_2_mm << endl;
            stream << "    " << -w_2_mm << " " << h_2_mm << endl;
            stream << "    " << w_2_mm << " " << h_2_mm << endl << endl;

            /// La relación entre unos parametros y otros es:
            /// K0' = 0;
            /// K1' = K1 / f³
            /// K2' = K2 / f⁵
            /// K3' = K3 / f⁷
            /// P1  = T1 / f²
            /// P2  = T2 / f²
            double f = camera.second.FocalLength() * scale;
            stream << "#Symmetrical Lens Distortion Odd-order Poly Coeffs:K0,K1,K2,K3" << endl;
            stream << "SYM_DIST 0 " <<
                      (model_id == 0 ? 0.0 : params[3] / pow(f, 3)) << " " <<
                      (model_id == 3 || model_id == 50 ? params[4] / pow(f, 5) : 0.0) << " " <<
                      (model_id == 50 ? params[5] / pow(f, 7) : 0.0) << endl << endl;

            stream << "#Decentering Lens Coeffs p1,p2,p3" << endl;
            stream << "DEC_DIST " <<
                      (model_id == 50 ? params[6] / f*f : 0.0) << " " <<
                      (model_id == 50 ? params[7] / f*f : 0.0) << " 0" << endl << endl;

            stream << "#How many distortion pairs (max 20):" << endl;
            stream << "NUM_DIST_PAIRS 20" << endl << endl;

          } else {
            ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
          }
        }

        file3.close();
      }

    } else {
      msgError("Unable to establish a database connection");
      return;
    }

    db.close();

  } else{
    msgError("There is not a valid reconstruction");
  }

}

void OrientationExport::exportMVE(const QString &path) const
{

  if (mReconstruction) {

    QDir dir(path);
    if (!dir.exists()) {
      if (dir.mkpath(".") == false) {
        msgError("The output directory cannot be created: %s", path.toStdString().c_str());
        return;
      }
    }

    QFile file(dir.filePath("synth_0.out"));

    /// TODO: Si no puede abrir el fichero tiene que devolver un error
    if (file.open(QIODevice::WriteOnly)) {
      QTextStream stream(&file);

      int camera_count = static_cast<int>(mReconstruction->Images().size());
      int feature_count = static_cast<int>(mReconstruction->NumPoints3D());

      stream << "drews 1.0" << endl;
                  stream << camera_count << " " << feature_count << endl;

      for (auto &camera : mReconstruction->Cameras()) {

        double sensor_width_px = std::max(camera.second.Width(), camera.second.Height());
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

        for (auto &image : mReconstruction->Images()) {
          if (image.second.CameraId() == camera.second.CameraId()) {

            std::string img_name = image.second.Name();

            Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
            Eigen::Vector3d translation = image.second.Tvec();

            QString ini_file(path);
            ini_file.append("\\views\\view_000").append(QString::number(image.second.ImageId()-1)).append(".mve");
            QDir dir_ini(ini_file);
            if (!dir_ini.exists()){
              if (dir_ini.mkpath(".") == false ){
                //msgWarning("The output directory cannot be created: %s", output_path.c_str());
              }
            }

            QFile file_view_ini(QDir(ini_file).filePath("meta.ini"));

            if (file_view_ini.open(QIODevice::WriteOnly)) {
              QTextStream stream_ini(&file_view_ini);

              stream_ini << "# MVE view meta data is stored in INI-file syntax." << endl;
              stream_ini << "# This file is generated, formatting will get lost." << endl << endl;
              stream_ini << "[camera]" << endl;
              stream_ini << "focal_length = " << focal << endl;
              stream_ini << "pixel_aspect = " << 1. << endl;
              //stream_ini << "principal_point = " << ppx << " " << ppy << endl;
              stream_ini << "principal_point = 0.5 0.5" << endl;
              stream_ini << "rotation = " << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << " "
                << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << " "
                << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << endl;
              stream_ini << "translation = " << translation[0] << " " << translation[1] << " " << translation[2] << endl << endl;
              stream_ini << "[view]" << endl;
              stream_ini << "id = " << image.second.ImageId()-1 << endl;
              stream_ini << "name = " << image.second.Name().c_str() << endl;

              file_view_ini.close();
            }

            ///TODO: ver porque pone 0 en la distorsión...
            //stream << focal << " " << "0" << " " << "0" << endl;  // Write '0' distortion values for pre-corrected images
            stream << focal << " " << (model_id == 0 ? 0 : params[3]) << " " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << endl;
            stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << endl;
            stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << endl;
            stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << endl;
            stream << translation[0] << " " << translation[1] << " " << translation[2] << endl;

            ///TODO: Se carga la imagen y se corrige de distorsión.
            //remap(left, left, map1, map2, INTER_LINEAR);

            //for (auto &points_3d : mReconstruction->Points3D()) {

            //  Eigen::Vector3ub color = points_3d.second.Color();
            //  stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << endl;
            //  // ¿??? stream << 250 << " " << 100 << " " << 150 << endl;  // Write arbitrary RGB color, see above note
            //  stream << color[0] << " " << color[1] << " " << color[2] << endl;

            //  colmap::Track track = points_3d.second.Track();
            //  stream << track.Length();

            //  for (auto &element : track.Elements()) {
            //    stream << " " << element.image_id << " " << element.point2D_idx << " 0";
            //  }
            //  stream << endl;
            //}

          }
        }
      }


      for (auto &points_3d : mReconstruction->Points3D()) {

        Eigen::Vector3ub color = points_3d.second.Color();
        stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << endl;

        //stream << 250 << " " << 100 << " " << 150 << endl;  // Write arbitrary RGB color, see above note
        stream << color[0] << " " << color[1] << " " << color[2] << endl;

        colmap::Track track = points_3d.second.Track();
        //stream << track.Length();

        //for (auto &element : track.Elements()) {
        //  stream << " " << element.image_id - 1 << " " << element.point2D_idx << " 0";
        //}
        std::map<int, int> track_ids_not_repeat;
        for (auto &element : track.Elements()) {
          track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
        }

        stream << track_ids_not_repeat.size();

        for (auto &map : track_ids_not_repeat) {
          stream << " " << map.first << " " << map.second << " 0";
        }


        stream << endl;
      }
      file.close();

    }
  }
}

void OrientationExport::exportBundler(const QString &oriFile, const QString &imageListFile) const
{
  if (mReconstruction)
    mReconstruction->ExportBundler(oriFile.toStdString(), imageListFile.toStdString());
  else
    msgError("There is not a valid reconstruction");
  //if (mReconstruction) {

  //  QFile file(filePath);

  //  TL_TODO("Si no puede abrir el fichero tiene que devolver un error")
  //  if (file.open(QIODevice::WriteOnly)) {

  //    QTextStream stream(&file);

  //    int camera_count = mReconstruction->Images().size();
  //    int feature_count = mReconstruction->NumPoints3D();

  //    stream << "# Bundle file v0.3" << endl;
  //    stream << camera_count << " " << feature_count << endl;

  //    for (auto &camera : mReconstruction->Cameras()) {

  //      double sensor_width_px = std::max(camera.second.Width(), camera.second.Height());
  //      double focal = camera.second.FocalLength() / sensor_width_px;
  //      int model_id = camera.second.ModelId();
  //      std::vector<double> params = camera.second.Params();

  //      for (auto &image : mReconstruction->Images()) {

  //        if (image.second.CameraId() == camera.second.CameraId()) {

  //          std::string img_name = image.second.Name();

  //          Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
  //          Eigen::Vector3d translation = image.second.Tvec();

  //          stream << focal << " " << (model_id == 0 ? 0 : params[3]) << " " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << endl;
  //          stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << endl;
  //          stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << endl;
  //          stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << endl;
  //          stream << translation[0] << " " << translation[1] << " " << translation[2] << endl;

  //        }
  //      }
  //    }


  //    for (auto &points_3d : mReconstruction->Points3D()) {

  //      Eigen::Vector3ub color = points_3d.second.Color();
  //      stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << endl;

  //      stream << color[0] << " " << color[1] << " " << color[2] << endl;

  //      colmap::Track track = points_3d.second.Track();

  //      std::map<int, int> track_ids_not_repeat;
  //      for (auto &element : track.Elements()) {
  //        track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
  //      }

  //      stream << track_ids_not_repeat.size();

  //      for (auto &map : track_ids_not_repeat) {
  //        stream << " " << map.first << " " << map.second << " 0";
  //      }


  //      stream << endl;
  //    }
  //    file.close();

  //  }
  //}
}

} // namespace inspector
