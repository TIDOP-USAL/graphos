#include "orientationexport.h"

#include <colmap/base/reconstruction.h>
#include <colmap/util/bitmap.h>
#include <colmap/util/misc.h>
#include <colmap/base/camera_database.h>

#include <tidop/core/messages.h>

//#include <QFile>
//#include <QTextStream>
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

struct PointPly 
{
  double x = 0.0f;
  double y = 0.0f;
  double z = 0.0f;
  double nx = 0.0f;
  double ny = 0.0f;
  double nz = 0.0f;
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
};

/// Extraido de colmap para poder generar un ply con puntos double

void writeBinaryPlyPoints(const std::string &path,
                          const std::vector<PointPly> &points,
                          bool write_normal, bool write_rgb)
{
  std::fstream text_file(path, std::ios::out);
  if (!text_file.is_open()) throw std::runtime_error(std::string("File not open: ").append(path));

  text_file << "ply" << std::endl;
  text_file << "format binary_little_endian 1.0" << std::endl;
  text_file << "element vertex " << points.size() << std::endl;

  text_file << "property double x" << std::endl;
  text_file << "property double y" << std::endl;
  text_file << "property double z" << std::endl;

  if (write_normal) {
    text_file << "property double nx" << std::endl;
    text_file << "property double ny" << std::endl;
    text_file << "property double nz" << std::endl;
  }

  if (write_rgb) {
    text_file << "property uchar red" << std::endl;
    text_file << "property uchar green" << std::endl;
    text_file << "property uchar blue" << std::endl;
  }

  text_file << "end_header" << std::endl;
  text_file.close();

  std::fstream binary_file(path,
                           std::ios::out | std::ios::binary | std::ios::app);
  if (!binary_file.is_open()) throw std::runtime_error(std::string("File not open: ").append(path));

  for (const auto& point : points) {
    colmap::WriteBinaryLittleEndian<double>(&binary_file, point.x);
    colmap::WriteBinaryLittleEndian<double>(&binary_file, point.y);
    colmap::WriteBinaryLittleEndian<double>(&binary_file, point.z);

    if (write_normal) {
      colmap::WriteBinaryLittleEndian<double>(&binary_file, point.nx);
      colmap::WriteBinaryLittleEndian<double>(&binary_file, point.ny);
      colmap::WriteBinaryLittleEndian<double>(&binary_file, point.nz);
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
    
    // Sólo exporta como float y en UTM se produce una perdida de precisión
    //mReconstruction->ExportPLY(path.toStdString());

    //QFileInfo info(path);
    //QString utm_ori = info.path() + "\\utm_ori" + info.suffix();
    std::vector<PointPly> ply_points;
    ply_points.reserve(mReconstruction->NumPoints3D());
    
    for (auto &point : mReconstruction->Points3D()) {
      PointPly ply_point;
      ply_point.x = point.second.X();
      ply_point.y = point.second.Y();
      ply_point.z = point.second.Z();
      ply_point.r = point.second.Color(0);
      ply_point.g = point.second.Color(1);
      ply_point.b = point.second.Color(2);
      ply_points.push_back(ply_point);
    }
    
    bool kWriteNormal = false;
    bool kWriteRGB = true;
    writeBinaryPlyPoints(path.toStdString(), ply_points, kWriteNormal, kWriteRGB);

  } else
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

void OrientationExport::exportOrientation(const QString &path, bool bQuaternion) const
{
  if (mReconstruction){
    /// Exportación personalizada
    //QFile file(path);

    std::ofstream stream(path.toStdString(), std::ios::trunc);
    if (stream.is_open()){
      //QTextStream stream(&file);

      if (bQuaternion){
        stream << "# IMAGE_ID QW QX QY QZ TX TY TZ CAMERA_ID NAME" << std::endl;
      } else{
        stream << "imageName X Y Z Omega Phi Kappa" << std::endl;
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
                    image.second.Name() << std::endl;
        } else {
          Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
          Eigen::Vector3d euler_angles = rotation_matrix.eulerAngles(0, 1, 2);

          stream << "\"" << image.second.Name() << "\" " <<
                    t[0] << " " <<
                    t[1] << " " <<
                    t[2] << " " <<
                    euler_angles[0] << " " <<
                    euler_angles[1] << " " <<
                    euler_angles[2] << " " << std::endl;
        }

      }
      stream.close();
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
    QString database_cameras_path;
#ifdef _DEBUG
    database_cameras_path = QString(INSPECTOR_SOURCE_PATH).append("/res");
#else
    database_cameras_path = qApp->applicationDirPath();
#endif
    database_cameras_path.append("/cameras.db");
    db.setDatabaseName(database_cameras_path);

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
        std::string file_path = dir.filePath("calibrated_internal_camera_parameters.cam").toStdString();
        std::ofstream stream(file_path, std::ios::trunc);
        if (stream.is_open()){

          int model_id = camera.second.ModelId();
          if (model_id == 0 || model_id == 2 || model_id == 3 || model_id == 50){

            size_t w = camera.second.Width();
            size_t h = camera.second.Height();
            //Eigen::Matrix3d calib_matrix = camera.second.CalibrationMatrix();
            std::vector<double> params = camera.second.Params();

            ///TODO: ¿El 0 se refiere a un identificador de cámara??
            stream << "Pix4D camera calibration file 0" << std::endl;
            stream << "#Focal Length mm assuming a sensor width of " <<
                      w * scale << "x" << h * scale << "mm" << std::endl;
            stream << "F " << camera.second.FocalLength() * scale << std::endl;

            stream << "#Principal Point mm" << std::endl;
            stream << "Px " << camera.second.PrincipalPointX()*scale << std::endl;
            stream << "Py " << camera.second.PrincipalPointY()*scale << std::endl;

            stream << "#Symmetrical Lens Distortion Coeffs" << std::endl;
            stream << "K1 " << (model_id == 0 ? 0.0 : params[3]) << std::endl;
            stream << "K2 " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << std::endl;
            stream << "K3 " << (model_id == 50 ? params[5] : 0.0) << std::endl;

            /// TODO: Controlar según el tipo de cámara.
            stream << "#Tangential Lens Distortion Coeffs" << std::endl;
            stream << "T1 " << (model_id == 50 ? params[6] : 0.0)  << std::endl;
            stream << "T2 " << (model_id == 50 ? params[7] : 0.0)  << std::endl << std::endl;
          } else {
            ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
          }
          stream.close();
        }


        file_path = dir.filePath("calibrated_internal_camera_parameters.cam").toStdString();
        stream.open(file_path, std::ios::trunc);
        if (stream.is_open()){

          int model_id = camera.second.ModelId();
          if (model_id == 0 || model_id == 2 || model_id == 3 || model_id == 50){

            size_t w = camera.second.Width();
            size_t h = camera.second.Height();
            //Eigen::Matrix3d calib_matrix = camera.second.CalibrationMatrix();
            std::vector<double> params = camera.second.Params();

            double w_2_mm = w * scale / 2.;
            double h_2_mm = h * scale / 2.;

            ///TODO: ¿El 0 se refiere a un identificador de cámara??
            stream << "camera_calibration_file 0" << std::endl;

            stream << "#Focal Length mm assuming a sensor width of " <<
                        w * scale << "x" << h * scale << "mm" << std::endl;
            stream << "#Image size " << w << "x" << h << " pixel" << std::endl;
            stream << "FOCAL " << camera.second.FocalLength() * scale << std::endl << std::endl;

            stream << "#Principal Point Offset xpoff ypoff in mm (Inpho)" << std::endl;
            stream << "XPOFF " << (camera.second.PrincipalPointX() * scale) - w_2_mm << std::endl;
            stream << "YPOFF " << (camera.second.PrincipalPointY() * scale) - h_2_mm << std::endl;
            stream << "#Principal Point Offset xpoff ypoff in mm" << std::endl;
            stream << "XPOFF " << w_2_mm - (camera.second.PrincipalPointX() * scale) << std::endl;
            stream << "YPOFF " << (camera.second.PrincipalPointY() * scale) - h_2_mm << std::endl;
            stream << "#Principal Point Offset xpoff ypoff in pixel" << std::endl;
            stream << "XPOFF " << w/2. - camera.second.PrincipalPointX() << std::endl;
            stream << "XPOFF " << camera.second.PrincipalPointY() - h/2. << std::endl  << std::endl;

            stream << "#How many fiducial pairs (max 8):" << std::endl;
            stream << "NUM_FIDS 4" << std::endl << std::endl;

            stream << "#Fiducials position" << std::endl;
            stream << "DATA_STRIP_SIDE left" << std::endl << std::endl;

            stream << "#Fiducial x,y pairs in mm:" << std::endl;
            stream << "FID_PAIRS" << std::endl;
            stream << "    " << w_2_mm << " " << -h_2_mm << std::endl;
            stream << "    " << -w_2_mm << " " << -h_2_mm << std::endl;
            stream << "    " << -w_2_mm << " " << h_2_mm << std::endl;
            stream << "    " << w_2_mm << " " << h_2_mm << std::endl << std::endl;

            /// La relación entre unos parametros y otros es:
            /// K0' = 0;
            /// K1' = K1 / f³
            /// K2' = K2 / f⁵
            /// K3' = K3 / f⁷
            /// P1  = T1 / f²
            /// P2  = T2 / f²
            double f = camera.second.FocalLength() * scale;
            stream << "#Symmetrical Lens Distortion Odd-order Poly Coeffs:K0,K1,K2,K3" << std::endl;
            stream << "SYM_DIST 0 " <<
                      (model_id == 0 ? 0.0 : params[3] / pow(f, 3)) << " " <<
                      (model_id == 3 || model_id == 50 ? params[4] / pow(f, 5) : 0.0) << " " <<
                      (model_id == 50 ? params[5] / pow(f, 7) : 0.0) << std::endl << std::endl;

            stream << "#Decentering Lens Coeffs p1,p2,p3" << std::endl;
            stream << "DEC_DIST " <<
                      (model_id == 50 ? params[6] / f*f : 0.0) << " " <<
                      (model_id == 50 ? params[7] / f*f : 0.0) << " 0" << std::endl << std::endl;

            stream << "#How many distortion pairs (max 20):" << std::endl;
            stream << "NUM_DIST_PAIRS 20" << std::endl << std::endl;

          } else {
            ///TODO: Cámara no soportada. ¿devolver error, escribir mensaje de error, ...?
          }
                  
          stream.close();
        }

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

    std::ofstream stream(dir.filePath("synth_0.out").toStdString(), std::ios::trunc);

    /// TODO: Si no puede abrir el fichero tiene que devolver un error
    if (stream.is_open()) {

      int camera_count = static_cast<int>(mReconstruction->Images().size());
      int feature_count = static_cast<int>(mReconstruction->NumPoints3D());

      stream << "drews 1.0" << std::endl;
                  stream << camera_count << " " << feature_count << std::endl;

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

        TL_TODO("imagenes registradas mejor")
        for (auto &image : mReconstruction->Images()) {
          if (image.second.CameraId() == camera.second.CameraId()) {

            //std::string img_name = image.second.Name();

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

            std::ofstream stream_ini(QDir(ini_file).filePath("meta.ini").toStdString(), std::ios::trunc);

            if (stream_ini.is_open()) {

              stream_ini << "# MVE view meta data is stored in INI-file syntax." << std::endl;
              stream_ini << "# This file is generated, formatting will get lost." << std::endl << std::endl;
              stream_ini << "[camera]" << std::endl;
              stream_ini << "focal_length = " << focal << std::endl;
              stream_ini << "pixel_aspect = " << 1. << std::endl;
              //stream_ini << "principal_point = " << ppx << " " << ppy << endl;
              stream_ini << "principal_point = 0.5 0.5" << std::endl;
              stream_ini << "rotation = " << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << " "
                << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << " "
                << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << std::endl;
              stream_ini << "translation = " << translation[0] << " " << translation[1] << " " << translation[2] << std::endl << std::endl;
              stream_ini << "[view]" << std::endl;
              stream_ini << "id = " << image.second.ImageId()-1 << std::endl;
              stream_ini << "name = " << image.second.Name().c_str() << std::endl;

              stream_ini.close();
            }

            ///TODO: ver porque pone 0 en la distorsión...
            /// Utiliza las imagenes corregidas... Volver a poner a 0....
            
            TL_TODO("El formato bundler r10, r11, r12, r20, r21, r22, T1 y T2 se invierte el signo!!! Aqui supongo que habría que hacerlo igual")
            //stream << focal << " " << "0" << " " << "0" << endl;  // Write '0' distortion values for pre-corrected images
            stream << focal << " " << (model_id == 0 ? 0 : params[3]) << " " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << std::endl;
            stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << std::endl;
            stream << rotation_matrix(1, 0) << " " << rotation_matrix(1, 1) << " " << rotation_matrix(1, 2) << std::endl;
            stream << rotation_matrix(2, 0) << " " << rotation_matrix(2, 1) << " " << rotation_matrix(2, 2) << std::endl;
            stream << translation[0] << " " << translation[1] << " " << translation[2] << std::endl;

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
        stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << std::endl;

        //stream << 250 << " " << 100 << " " << 150 << endl;  // Write arbitrary RGB color, see above note
        stream << static_cast<int>(color[0]) << " " <<
                  static_cast<int>(color[1]) << " " <<
                  static_cast<int>(color[2]) << std::endl;

        colmap::Track track = points_3d.second.Track();

        std::map<int, int> track_ids_not_repeat;
        for (auto &element : track.Elements()) {
          track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;        }

        stream << track_ids_not_repeat.size();

        for (auto &map : track_ids_not_repeat) {
          stream << " " << map.first << " " << map.second << " 0";
        }


        stream << std::endl;
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
        msgError("The output directory cannot be created: %s", path.toStdString().c_str());
        return;
      }
    }


  }
}

void OrientationExport::exportBundler(const QString &oriFile) const
{
  ///TODO: ¿En que cambiaba de generarlo yo o Colmap?

  //if (mReconstruction)
  //  mReconstruction->ExportBundler(oriFile.toStdString(), imageListFile.toStdString());
  //else
  //  msgError("There is not a valid reconstruction");



//#ifdef _DEBUG
//  /// Borrar
//  if (mReconstruction) {
//    const colmap::Image image = mReconstruction->Image(static_cast<colmap::image_t>(3));
//    std::string name = image.Name().c_str();
//    msgInfo(image.Name().c_str());
//    colmap::camera_t camera_id = image.CameraId();
//    colmap::Camera camera =mReconstruction->Camera(camera_id);
//
//    for (auto &points_3d : mReconstruction->Points3D()) {
//      colmap::point3D_t point_3d_id = points_3d.first;
//      const colmap::Point3D point_3d = points_3d.second;
//      if (image.HasPoint3D(point_3d_id)){
//        colmap::Track track = point_3d.Track();
//        for (auto &element : track.Elements()) {
//          colmap::image_t image_id = element.image_id;
//          if (image_id == image.ImageId()) {
//            colmap::point2D_t point_2d_id = element.point2D_idx;
//            const colmap::Point2D point_2d = image.Point2D(point_2d_id);
//            msgInfo("3D Point: (%lf;%lf;%lf)", point_3d.X(), point_3d.Y(), point_3d.Z());
//            msgInfo("2D Point: (%lf;%lf)", point_2d.X(), point_2d.Y());
//            Eigen::Vector3d point3D = point_3d.XYZ();
//            Eigen::Matrix3x4d proj_matrix = image.ProjectionMatrix();
//            Eigen::Vector2d image_point = colmap::ProjectPointToImage(point3D, proj_matrix, camera);
//            msgInfo("Project Point: (%lf;%lf)", image_point[0], image_point[1]);
//            double x = image_point[0];
//            double y = image_point[1];
//            double x_dif = x - point_2d.X();
//            double y_dif = y - point_2d.Y();
//            msgInfo("dif: %lf;%lf", x_dif, y_dif);
//
//            //Eigen::Vector3d point3D_2(271998 - 272021.250, 4338425.7 - 4338368.076, 351.916 - 379.370);
//            Eigen::Vector3d point3D_2(-22.9325, 57.457, -27.328);
//            Eigen::Vector2d image_point_2 = colmap::ProjectPointToImage(point3D_2, proj_matrix, camera);
//
//            msgInfo("3D Point: (%lf;%lf;%lf)", point3D_2[0], point3D_2[1], point3D_2[2]);
//            msgInfo("2D Point: (%lf;%lf)", image_point_2[0], image_point_2[1]);
//
//            const Eigen::Matrix<double, 3, 4> inv_proj_matrix = image.InverseProjectionMatrix();
//            const Eigen::Vector3d point3D_3 = inv_proj_matrix.rightCols<1>();
//            Eigen::Vector2d image_point_3 = colmap::ProjectPointToImage(point3D_3, proj_matrix, camera);
//                        
//            msgInfo("3D Point: (%lf;%lf;%lf)", point3D_3[0], point3D_3[1], point3D_3[2]);
//            msgInfo("2D Point: (%lf;%lf)", image_point_3[0], image_point_3[1]);
//            return;
//          }
//
//        }
//      }
//
//    }
//
//
//  }
//#endif



  if (mReconstruction) {

    std::ofstream stream(oriFile.toStdString(), std::ios::trunc);
    if (stream.is_open()) {

      int camera_count = mReconstruction->Images().size();
      int feature_count = mReconstruction->NumPoints3D();

      stream << "# Bundle file v0.3" << std::endl;
      stream << camera_count << " " << feature_count << std::endl;

      for (auto &camera : mReconstruction->Cameras()) {

        double sensor_width_px = std::max(camera.second.Width(), camera.second.Height());
        double focal = camera.second.FocalLength() / sensor_width_px;
        int model_id = camera.second.ModelId();
        std::vector<double> params = camera.second.Params();

        for (auto &image : mReconstruction->Images()) {

          if (image.second.CameraId() == camera.second.CameraId()) {

            std::string img_name = image.second.Name();

            Eigen::Matrix3d rotation_matrix = image.second.RotationMatrix();
            Eigen::Vector3d translation = image.second.Tvec();
            TL_TODO("El formato bundler r10, r11, r12, r20, r21, r22, T1 y T2 se invierte el signo!!! Aqui supongo que habría que hacerlo igual")
            stream << focal << " " << (model_id == 0 ? 0 : params[3]) << " " << (model_id == 3 || model_id == 50 ? params[4] : 0.0) << std::endl;
            stream << rotation_matrix(0, 0) << " " << rotation_matrix(0, 1) << " " << rotation_matrix(0, 2) << std::endl;
            stream << -rotation_matrix(1, 0) << " " << -rotation_matrix(1, 1) << " " << -rotation_matrix(1, 2) << std::endl;
            stream << -rotation_matrix(2, 0) << " " << -rotation_matrix(2, 1) << " " << -rotation_matrix(2, 2) << std::endl;
            stream << translation[0] << " " << -translation[1] << " " << -translation[2] << std::endl;

          }
        }
      }


      for (auto &points_3d : mReconstruction->Points3D()) {

        Eigen::Vector3ub color = points_3d.second.Color();
        stream << points_3d.second.X() << " " << points_3d.second.Y() << " " << points_3d.second.Z() << std::endl;

        stream << static_cast<int>(color[0]) << " " <<
                  static_cast<int>(color[1]) << " " <<
                  static_cast<int>(color[2]) << std::endl;

        colmap::Track track = points_3d.second.Track();

        std::map<int, int> track_ids_not_repeat;
        for (auto &element : track.Elements()) {
          track_ids_not_repeat[element.image_id - 1] = element.point2D_idx;
        }

        stream << track_ids_not_repeat.size();

        for (auto &map : track_ids_not_repeat) {

          const colmap::Image &image = mReconstruction->Image(map.first+1);
          const colmap::Camera &camera = mReconstruction->Camera(image.CameraId());

          const colmap::Point2D &point2D = image.Point2D(map.second);

          stream << " " << map.first << " " << map.second << " ";
          stream << point2D.X() - camera.PrincipalPointX() << " ";
          stream << camera.PrincipalPointY() - point2D.Y() << " ";
        }

        stream << std::endl;
      }
      stream.close();

    }
  } else 
    msgError("There is not a valid reconstruction");
}

} // namespace inspector
