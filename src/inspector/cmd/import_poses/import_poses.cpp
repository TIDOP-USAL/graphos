#include <inspector/core/utils.h>
#include <inspector/core/project.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>
#include <tidop/core/path.h>
#include <tidop/geospatial/crs.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/math/algebra/euler_angles.h>
#include <tidop/math/algebra/quaternion.h>
#include <tidop/math/algebra/rotation_convert.h>
#include <tidop/math/math.h>


#include <QDir>
#include <QTextStream>

using namespace inspector;
using namespace tl;
using namespace geospatial;




math::RotationMatrix<double> rotationMatrixEnu(double longitude, double latitude)
{
  math::RotationMatrix<double> rotation_enu;

  double longitude_rad = longitude * math::consts::deg_to_rad<double>;
  double latitude_rad = latitude * math::consts::deg_to_rad<double>;

  double sin_longitude = sin(longitude_rad);
  double cos_longitude = cos(longitude_rad);
  double sin_latitude = sin(latitude_rad);
  double cos_latitude = cos(latitude_rad);

  rotation_enu.at(0, 0) = -sin_longitude;
  rotation_enu.at(0, 1) = cos_longitude;
  rotation_enu.at(0, 2) = 0;
  rotation_enu.at(1, 0) = -sin_latitude * cos_longitude;
  rotation_enu.at(1, 1) = -sin_latitude * sin_longitude;
  rotation_enu.at(1, 2) = cos_latitude;
  rotation_enu.at(2, 0) = cos_latitude * cos_longitude;
  rotation_enu.at(2, 1) = cos_latitude * sin_longitude;
  rotation_enu.at(2, 2) = sin_latitude;

  return rotation_enu;
}

math::Vector<double, 3> ecefToEnu(const Point3D center_geocentric,
                                  const Point3D camera_geocentric,
                                  const math::RotationMatrix<double> &rotation)
{
  Point3D t = camera_geocentric - center_geocentric;

  math::Vector<double, 3> enu = rotation * t.vector();

  return enu;
}

math::Vector<double, 3> ecefToEnu(const Point3D center_geocentric,
                                  const Point3D camera_geocentric,
                                  double longitude, double latitude)
{
  math::RotationMatrix<double> rotation = rotationMatrixEnu(longitude, latitude);
  Point3D dif = camera_geocentric - center_geocentric;

  math::Vector<double, 3> enu = rotation * dif.vector();

  return enu;
}

Point3D enuToEcef(const Point3D enu, const Point3D center_geocentric, double longitude, double latitude)
{
  math::RotationMatrix<double> rotation = rotationMatrixEnu(longitude, latitude);
  math::Vector<double, 3> d = rotation.transpose() * enu.vector();

  Point3D pt_geocentric;
  pt_geocentric.x = center_geocentric.x + d[0];
  pt_geocentric.y = center_geocentric.y + d[1];
  pt_geocentric.z = center_geocentric.z + d[2];

  return pt_geocentric;
}





int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "Import Poses";

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

/* Parseo de argumentos */

  std::string project_file{};
  std::string input{};
  std::string epsg = "NONE";
  std::string rotation = "NONE";
  std::string separator = ";";

  Command cmd("import_poses", cmd_description);
  cmd.push_back(std::make_shared<ArgumentStringRequired>("prj", 'p', "Project file", &project_file));
  cmd.push_back(std::make_shared<ArgumentStringRequired>("input", 'i', "Poses file (id,x,y,z[,rotation])", &input));
  cmd.push_back(std::make_shared<ArgumentStringOptional>("epsg", 'e', "EPSG. Por defecto ';'", &epsg));
  cmd.push_back(std::make_shared<ArgumentStringOptional>("rotation", 'r', "(axis_angle, euler_angles, rotation_matrix, quaternion)", &rotation));
  cmd.push_back(std::make_shared<ArgumentStringOptional>("separator", 's', "Caracter separador. Por defecto ';'", &separator));


  cmd.addExample("import_poses --prj 253/253.xml --input poses.txt --epsg EPSG:25830 --rotation rotation_matrix");

  // Parseo de los argumentos y comprobación de los mismos
  Command::Status status = cmd.parse(argc, argv);
  if (status == Command::Status::parse_error ) {
    return 1;
  } else if (status == Command::Status::show_help) {
    return 0;
  } else if (status == Command::Status::show_licence) {
    return 0;
  } else if (status == Command::Status::show_version) {
    return 0;
  }

  bool bConvertToLocal = false;

  std::shared_ptr<Crs> epsgGeographic(new Crs("EPSG:4258"));
  std::shared_ptr<Crs> epsgGeocentric(new Crs("EPSG:4936"));
  std::shared_ptr<Crs> epsgUTM;
  std::unique_ptr<CrsTransform<Point3D>> crsTransformUtmToGeographic;
  std::unique_ptr<CrsTransform<Point3D>> crsTransformUtmToGeocentric;

  if (epsg != "NONE") {

    bConvertToLocal = true;

    epsgUTM = std::make_shared<Crs>(epsg);
    crsTransformUtmToGeographic = std::make_unique<CrsTransform<Point3D>>(epsgUTM, epsgGeographic);
    crsTransformUtmToGeocentric = std::make_unique<CrsTransform<Point3D>>(epsgUTM, epsgGeocentric);
  }

  std::map<std::string, CameraPose> camera_poses;

  try {

    /// Se lee el proyecto

    ProjectImp project;
    project.load(project_file.c_str());

    /// Se leen las orientaciones de las cámaras

    std::ifstream ifs;
    ifs.open(input, std::ifstream::in);

    //std::vector<Point3D> ptos_utm; 
    //std::vector<Point3D> ptos_local;

    if (ifs.is_open()) {

      std::string line;
      std::getline(ifs, line);
      std::getline(ifs, line);

      int i = 1;
      Point3D offset(0., 0., 0.);

      while (std::getline(ifs, line)) {

        CameraPose camera_pose;

        // image name
        std::vector<std::string> vector_out;
        tl::split(line, vector_out, separator.c_str());
        std::string image_name = vector_out[0];

        // Camera position

        camera_pose.position.x = stod(vector_out[1]);
        camera_pose.position.y = stod(vector_out[2]);
        camera_pose.position.z = stod(vector_out[3]);

        offset += (camera_pose.position - offset) / i;

        // Camera orientation

        if (rotation == "euler_angles"){ 
        
          /// Omega Phi Kappa

          math::EulerAngles<double> opk;
          opk.x = stod(vector_out[4]) * math::consts::deg_to_rad<double>;
          opk.y = stod(vector_out[5]) * math::consts::deg_to_rad<double>;
          opk.z = stod(vector_out[6]) * math::consts::deg_to_rad<double>;
          
          tl::math::RotationConverter<double>::convert(opk, camera_pose.rotation);

        } else if (rotation == "quaternion") {

          math::Quaterniond quaternion;
          quaternion.x = stod(vector_out[4]);
          quaternion.y = stod(vector_out[5]);
          quaternion.z = stod(vector_out[6]);
          quaternion.w = stod(vector_out[7]);

          tl::math::RotationConverter<double>::convert(quaternion, camera_pose.rotation);

        } else {

          /// Matriz de Rotación

          camera_pose.rotation.at(0, 0) = stod(vector_out[4]);
          camera_pose.rotation.at(0, 1) = stod(vector_out[5]);
          camera_pose.rotation.at(0, 2) = stod(vector_out[6]);
          camera_pose.rotation.at(1, 0) = stod(vector_out[7]);
          camera_pose.rotation.at(1, 1) = stod(vector_out[8]);
          camera_pose.rotation.at(1, 2) = stod(vector_out[9]);
          camera_pose.rotation.at(2, 0) = stod(vector_out[10]);
          camera_pose.rotation.at(2, 1) = stod(vector_out[11]);
          camera_pose.rotation.at(2, 2) = stod(vector_out[12]);

        }

        camera_poses[image_name] = camera_pose;

        i++;
      }

      ifs.close();

      /// Conversión al sistema de Colmap

      /// Paso a coordenadas geograficas

      Point3D center_geographic = crsTransformUtmToGeographic->transform(offset);

      /// Paso a coordenadas geocentricas

      Point3D center_geocentric = crsTransformUtmToGeocentric->transform(offset);

      math::RotationMatrix<double> rotation_enu = rotationMatrixEnu(center_geographic.y, center_geographic.x);



      for (const auto &camera : camera_poses) {
      
        std::string image_name = camera.first;

        for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
          
          std::string file_name = Path(image->path().toStdString()).fileName();
          if (file_name == image_name) {

            CameraPosition cameraPosition;

            math::Vector<double, 3> vector_camera_position;
            math::RotationMatrix<double> rotation_matrix = camera.second.rotation;

            if (bConvertToLocal) {
              
              /// No funciona bien el paso a locales. Por ahora se hace traslación al baricentro
              Point3D point = camera.second.position - offset;
              vector_camera_position = point.vector();
              ///// Paso a coordenadas geocentricas

              //Point3D point_geocentric = crsTransformUtmToGeocentric->transform(camera.second.position);

              ///// Paso coodenadas geocentricas (ECEF) a coordenadas ENU

              //vector_camera_position = ecefToEnu(center_geocentric, point_geocentric, rotation_enu);

            } else {

              vector_camera_position = camera.second.position.vector();

            }

            math::RotationMatrix<double> r_ip_ic = math::RotationMatrix<double>::identity();
            r_ip_ic.at(1, 1) = -1;
            r_ip_ic.at(2, 2) = -1;

            math::RotationMatrix<double> rotation = r_ip_ic * rotation_matrix.transpose();
            tl::math::Quaterniond quaternion;
            tl::math::RotationConverter<double>::convert(rotation, quaternion);
            quaternion.normalize();

            vector_camera_position = rotation * -vector_camera_position;

            cameraPosition.setX(vector_camera_position[0]);
            cameraPosition.setY(vector_camera_position[1]);
            cameraPosition.setZ(vector_camera_position[2]);
            cameraPosition.setSource(input.c_str());
            cameraPosition.setQuaternion(quaternion);
            cameraPosition.setCrs(epsg.c_str());
            // Tengo una clase CameraPose y otra CameraPosition... Tratar de reorganizar el proyecto
            image->setCameraPosition(cameraPosition);

            break;
          }
        }

      }

      /// Triangulación y ajuste de haces. Hay que extraerlo de colmap y segun se fije o no la focal, el punto principal y los parametros de calibración
      /// se liberan o no. Tambien hay que ver las rotaciones
      /// Generar los ficheros txt en formato colmap para importar las camaras
    
    
    }

    project.save(project_file.c_str());

  } catch (std::exception &e) {
    msgError(e.what());
  }

  return 0;
}
