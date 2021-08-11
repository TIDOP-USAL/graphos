#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/orientation/orientationexport.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>
#include <tidop/core/chrono.h>
#include <tidop/core/path.h>
#include <tidop/geospatial/crs.h>
#include <tidop/geospatial/crstransf.h>
//#include <tidop/geometry/entities/point.h>
//#include <tidop/math/algebra/euler_angles.h>
#include <tidop/math/algebra/quaternion.h>
#include <tidop/math/algebra/rotation_convert.h>
//#include <tidop/math/math.h>

#include <colmap/util/option_manager.h>
#include <colmap/base/reconstruction.h>
#include <colmap/base/database.h>
#include <colmap/base/database_cache.h>
#include <colmap/controllers/incremental_mapper.h>
#include <colmap/base/camera_models.h>
#include <colmap/base/cost_functions.h>
#include <colmap/util/misc.h>

#include <QDir>
#include <QTextStream>

using namespace inspector;
using namespace tl;
using namespace geospatial;

namespace inspector
{

class BundleAdjuster
  : public colmap::BundleAdjuster
{

public:

  BundleAdjuster(const colmap::BundleAdjustmentOptions &options,
                        const colmap::BundleAdjustmentConfig &config)
    : colmap::BundleAdjuster(options,
                             config)
  {}

  ~BundleAdjuster(){ }

  bool Solve(colmap::Reconstruction *reconstruction)
  {
    CHECK_NOTNULL(reconstruction);
    CHECK(!problem_) << "Cannot use the same BundleAdjuster multiple times";

    problem_.reset(new ceres::Problem());

    ceres::LossFunction *loss_function = options_.CreateLossFunction();
    // Warning: AddPointsToProblem assumes that AddImageToProblem is called first.
    // Do not change order of instructions!
    for (const colmap::image_t image_id : config_.Images()) {
      AddImageToProblem(image_id, reconstruction, loss_function);
    }
    for (const auto point3D_id : config_.VariablePoints()) {
      AddPointToProblem(point3D_id, reconstruction, loss_function);
    }
    for (const auto point3D_id : config_.ConstantPoints()) {
      AddPointToProblem(point3D_id, reconstruction, loss_function);
    }

    ParameterizeCameras(reconstruction);
    ParameterizePoints(reconstruction);

    if (problem_->NumResiduals() == 0) {
      return false;
    }

    ceres::Solver::Options solver_options = options_.solver_options;
    const bool has_sparse =
      solver_options.sparse_linear_algebra_library_type != ceres::NO_SPARSE;

    // Empirical choice.
    const size_t kMaxNumImagesDirectDenseSolver = 50;
    const size_t kMaxNumImagesDirectSparseSolver = 1000;
    const size_t num_images = config_.NumImages();
    if (num_images <= kMaxNumImagesDirectDenseSolver) {
      solver_options.linear_solver_type = ceres::DENSE_SCHUR;
    } else if (num_images <= kMaxNumImagesDirectSparseSolver && has_sparse) {
      solver_options.linear_solver_type = ceres::SPARSE_SCHUR;
    } else {  // Indirect sparse (preconditioned CG) solver.
      solver_options.linear_solver_type = ceres::ITERATIVE_SCHUR;
      solver_options.preconditioner_type = ceres::SCHUR_JACOBI;
    }

    if (problem_->NumResiduals() <
        options_.min_num_residuals_for_multi_threading) {
      solver_options.num_threads = 1;
#if CERES_VERSION_MAJOR < 2
      solver_options.num_linear_solver_threads = 1;
#endif  // CERES_VERSION_MAJOR
    } else {
      solver_options.num_threads =
        colmap::GetEffectiveNumThreads(solver_options.num_threads);
#if CERES_VERSION_MAJOR < 2
      solver_options.num_linear_solver_threads =
        colmap::GetEffectiveNumThreads(solver_options.num_linear_solver_threads);
#endif  // CERES_VERSION_MAJOR
    }

    std::string solver_error;
    CHECK(solver_options.IsValid(&solver_error)) << solver_error;

    ceres::Solve(solver_options, problem_.get(), &summary_);

    if (solver_options.minimizer_progress_to_stdout) {
      std::cout << std::endl;
    }

    if (options_.print_summary) {
      colmap::PrintHeading2("Bundle adjustment report");
      colmap::PrintSolverSummary(summary_);
    }

    //TearDown(reconstruction);

    return true;
  }

protected:

  void AddImageToProblem(const colmap::image_t image_id,
                         colmap::Reconstruction *reconstruction,
                         ceres::LossFunction *loss_function)
  {
    
    colmap::Image &image = reconstruction->Image(image_id);
    colmap::Camera &camera = reconstruction->Camera(image.CameraId());

    // CostFunction assumes unit quaternions.
    image.NormalizeQvec();

    double *qvec_data = image.Qvec().data();
    double *tvec_data = image.Tvec().data();
    double *camera_params_data = camera.ParamsData();

    const bool constant_pose =
      !options_.refine_extrinsics || config_.HasConstantPose(image_id);

    // Add residuals to bundle adjustment problem.
    size_t num_observations = 0;
    for (const colmap::Point2D &point2D : image.Points2D()) {
      if (!point2D.HasPoint3D()) {
        continue;
      }

      num_observations += 1;
      point3D_num_observations_[point2D.Point3DId()] += 1;

      colmap::Point3D &point3D = reconstruction->Point3D(point2D.Point3DId());
      assert(point3D.Track().Length() > 1);

      ceres::CostFunction *cost_function = nullptr;

      if (constant_pose) {
        switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                 \
  case colmap::CameraModel::kModelId:                                          \
    cost_function =                                                    \
        colmap::BundleAdjustmentConstantPoseCostFunction<colmap::CameraModel>::Create( \
            image.Qvec(), image.Tvec(), point2D.XY());                 \
    break;

          CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
        }

        problem_->AddResidualBlock(cost_function, loss_function,
                                   point3D.XYZ().data(), camera_params_data);
      } else {
        switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                   \
  case colmap::CameraModel::kModelId:                                            \
    cost_function =                                                      \
        colmap::BundleAdjustmentCostFunction<colmap::CameraModel>::Create(point2D.XY()); \
    break;

          CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
        }

        problem_->AddResidualBlock(cost_function, loss_function, qvec_data,
                                   tvec_data, point3D.XYZ().data(),
                                   camera_params_data);
      }
    }

    if (num_observations > 0) {
      camera_ids_.insert(image.CameraId());

      problem_->SetParameterBlockConstant(tvec_data);
      // Set pose parameterization.
      if (!constant_pose) {
        ceres::LocalParameterization *quaternion_parameterization =
          new ceres::QuaternionParameterization;
        problem_->SetParameterization(qvec_data, quaternion_parameterization);
        
        //if (config_.HasConstantTvec(image_id)) {
        //  const std::vector<int> &constant_tvec_idxs =
        //    config_.ConstantTvec(image_id);
        //  ceres::SubsetParameterization *tvec_parameterization =
        //    new ceres::SubsetParameterization(3, constant_tvec_idxs);
        //  problem_->SetParameterization(tvec_data, tvec_parameterization);
        //}
      }
    }
  }

  void AddPointToProblem(const colmap::point3D_t point3D_id,
                         colmap::Reconstruction *reconstruction,
                         ceres::LossFunction *loss_function)
  {
    colmap::Point3D &point3D = reconstruction->Point3D(point3D_id);

    // Is 3D point already fully contained in the problem? I.e. its entire track
    // is contained in `variable_image_ids`, `constant_image_ids`,
    // `constant_x_image_ids`.
    if (point3D_num_observations_[point3D_id] == point3D.Track().Length()) {
      return;
    }

    for (const auto &track_el : point3D.Track().Elements()) {
      // Skip observations that were already added in `FillImages`.
      if (config_.HasImage(track_el.image_id)) {
        continue;
      }

      point3D_num_observations_[point3D_id] += 1;

      colmap::Image &image = reconstruction->Image(track_el.image_id);
      colmap::Camera &camera = reconstruction->Camera(image.CameraId());
      const colmap::Point2D &point2D = image.Point2D(track_el.point2D_idx);

      // We do not want to refine the camera of images that are not
      // part of `constant_image_ids_`, `constant_image_ids_`,
      // `constant_x_image_ids_`.
      if (camera_ids_.count(image.CameraId()) == 0) {
        camera_ids_.insert(image.CameraId());
        config_.SetConstantCamera(image.CameraId());
      }

      ceres::CostFunction *cost_function = nullptr;

      switch (camera.ModelId()) {
#define CAMERA_MODEL_CASE(CameraModel)                                 \
  case colmap::CameraModel::kModelId:                                          \
    cost_function =                                                    \
        colmap::BundleAdjustmentConstantPoseCostFunction<colmap::CameraModel>::Create( \
            image.Qvec(), image.Tvec(), point2D.XY());                 \
    break;

        CAMERA_MODEL_SWITCH_CASES

#undef CAMERA_MODEL_CASE
      }
      problem_->AddResidualBlock(cost_function, loss_function,
                                 point3D.XYZ().data(), camera.ParamsData());
    }
  }

private:

};

}




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


/// -p "C:\Users\esteban\Documents\Inspector\Projects\Madrigalejo\zona1_thermal\zona1_thermal.xml" --fix_calibration
/// -p "C:\Users\esteban\Documents\Inspector\Projects\Madrigalejo\zona1_thermal\zona1_thermal.xml" --fix_calibration

int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "Orientation";

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

/* Parseo de argumentos */

  Path project_file{};
  bool enu = false;
  bool fix_calibration = false;

  Command cmd("orientation", cmd_description);
  cmd.push_back(std::make_shared<ArgumentPathRequired>("prj", 'p', "Project file", &project_file));
  cmd.push_back(std::make_shared<ArgumentBooleanOptional>("enu", 'e', "Convert to ENU", &enu));
  cmd.push_back(std::make_shared<ArgumentBooleanOptional>("fix_calibration", "Deja fijos los parámetros de calibración", &fix_calibration));

  cmd.addExample("orientation --prj 253/253.xml");

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

  ProjectImp project;
  project.load(project_file.toString().c_str());
  
  /// 1 - reconstrucción temporal con orientaciones importadas
  tl::Path input_path("C:\\temp\\inspector\\reconstruction_import");
  //tl::Path input_path = tl::Path::tempDirectory(); // Tiene que ser un directorio temporal en el que se exporte la reconstrucción como texto.
  //input_path.append("import_ori");
  input_path.createDirectory();

  bool bLocalCoord = true;
  Point3D offset(0., 0., 0.);
  /// leer imagenes y escribir images.txt en reconstrucción temporal
  //{
  //  offset = Point3D(272020.115, 4338394.551, 321.311);
  //  bLocalCoord = false;
  //  // --import C:\Users\esteban\Documents\Inspector\Projects\Madrigalejo\ori\absolute\images.txt 
  //  // --list C:\Users\esteban\Documents\Inspector\prueba_ori_termico\rgb_termico.txt --export C:\temp\inspector\reconstruction_import\images.txt
  //  Path import_ori("C:\\Users\\esteban\\Documents\\Inspector\\Projects\\Madrigalejo\\ori\\absolute\\images.txt");
  //  Path images("C:\\Users\\esteban\\Documents\\Inspector\\prueba_ori_termico\\rgb_termico.txt");
  //  Path images_path(input_path);
  //  images_path.append("images.txt");

  //  if (!import_ori.exists()) throw std::runtime_error("");
  //  if (!images.exists()) throw std::runtime_error("");

  //  std::map<std::string, std::string> map_thermal;

  //  std::ifstream ifs;
  //  ifs.open(images.toString(), std::ifstream::in);
  //  if (ifs.is_open()) {

  //    std::string rgb;
  //    std::string thermal;
  //    while (std::getline(ifs, rgb)) {
  //      std::getline(ifs, thermal);
  //      map_thermal[rgb] = thermal;
  //    }
  //    ifs.close();
  //  }

  //  ifs.open(import_ori.toString(), std::ifstream::in);
  //  std::ofstream ofs;
  //  ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);
  //  if (ifs.is_open() && ofs.is_open()) {

  //    std::string line;
  //    std::getline(ifs, line);
  //    std::stringstream string_stream(line);

  //    ofs << line << "\n";

  //    std::getline(ifs, line);
  //    ofs << line << "\n";

  //    std::getline(ifs, line);
  //    ofs << line << "\n";

  //    std::getline(ifs, line);
  //    ofs << line << "\n";

  //    int image_id_in;
  //    double qw{};
  //    double qx{};
  //    double qy{};
  //    double qz{};
  //    double x{};
  //    double y{};
  //    double z{};
  //    int camera;

  //    std::string image;

  //    int image_id = 0;

  //    while (std::getline(ifs, line)) {

  //      string_stream.str(line);
  //      string_stream.clear();

  //      string_stream >> image_id_in >> qw >> qx >> qy >> qz >> x >> y >> z >> camera >> image;

  //      Path path_image(image);
  //      auto it = map_thermal.find(path_image.fileName());
  //      if (it != map_thermal.end()) {
  //        path_image.replaceFileName(it->second);

  //        if (project.existImage(path_image.baseName().c_str())) {

  //          image_id = project.imageId(path_image.baseName().c_str());
  //          Image _image = project.findImageById(image_id);
  //          
  //          ofs << std::fixed << image_id+1 << " " << qw << " " << qx << " " << qy << " " << qz << " " << x << " " << y << " " << z << " " << 1 << " " << _image.path().toStdString().c_str() << "\n\n";

  //        }

  //      }

  //      std::getline(ifs, line);
  //    }
  //    ofs << std::endl;

  //    ifs.close();
  //    ofs.close();
  //  }
  //}
  {
    tl::Path images_path(input_path);
    images_path.append("images.txt");
  
    std::ofstream ofs;
    ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);
  
    if (!ofs.is_open()) return 1;
  
    
    auto it = project.imageBegin();
    if (it != project.imageEnd()) {
      CameraPosition cameraPosition = it->cameraPosition();
      if (cameraPosition.crs() != "") bLocalCoord = false;
    }
  
    int i = 1;
    for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
      CameraPosition cameraPosition = image->cameraPosition();
      if (cameraPosition.isEmpty()) continue;
      tl::Point3D position(cameraPosition.x(), cameraPosition.y(), cameraPosition.z());
      offset += (position - offset) / i;
      i++;
    }
     
    size_t id = 1;
  
    if (enu) {
  
      std::shared_ptr<Crs> epsgGeographic(new Crs("EPSG:4258"));
      std::shared_ptr<Crs> epsgGeocentric(new Crs("EPSG:4936"));
      std::shared_ptr<Crs> epsgUTM(new Crs("EPSG:25830"));
      CrsTransform<Point3D> crsTransformUtmToGeographic(epsgUTM, epsgGeographic);
      CrsTransform<Point3D> crsTransformUtmToGeocentric(epsgUTM, epsgGeocentric);
  
      /// Paso a coordenadas geograficas
  
      Point3D center_geographic = crsTransformUtmToGeographic.transform(offset);
  
      /// Paso a coordenadas geocentricas
  
      Point3D center_geocentric = crsTransformUtmToGeocentric.transform(offset);
  
      math::RotationMatrix<double> rotation_enu = rotationMatrixEnu(center_geographic.y, center_geographic.x);
  
  
      for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
  
        CameraPosition cameraPosition = image->cameraPosition();
        if (cameraPosition.isEmpty()) {
          id++;
          continue; /// Se saltan las imagenes no orientadas
        }
  
        math::Quaternion<double> quaternion = cameraPosition.quaternion();
        /// Colmap da problemas con el formato texto si la ruta del fichero tiene espacios...
        std::string file_name = image->path().toStdString();//Path(image->path().toStdString()).fileName();
        tl::Point3D position(cameraPosition.x(), cameraPosition.y(), cameraPosition.z());
  
        /// Paso a coordenadas geocentricas
  
        Point3D point_geocentric = crsTransformUtmToGeocentric.transform(position);
  
        /// Paso coodenadas geocentricas (ECEF) a coordenadas ENU
  
        math::Vector<double, 3> vector_camera_position = ecefToEnu(center_geocentric, point_geocentric, rotation_enu);
      
        math::RotationMatrix<double> rotation_matrix;
        math::RotationConverter<double>::convert(quaternion, rotation_matrix);
  
        math::RotationMatrix<double> r_ip_ic = math::RotationMatrix<double>::identity();
        //r_ip_ic.at(0, 0) = -1;
        r_ip_ic.at(1, 1) = -1;
        r_ip_ic.at(2, 2) = -1;
  
        math::RotationMatrix<double> rotation = r_ip_ic * rotation_matrix.transpose();
        vector_camera_position = rotation * -vector_camera_position;
        tl::math::RotationConverter<double>::convert(rotation, quaternion);
        quaternion.normalize();
  
        ofs << std::fixed << id++ << " " << quaternion.w << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << " " <<
          vector_camera_position[0] << " " << vector_camera_position[1] << " " << vector_camera_position[2] << " 1 " << file_name << std::endl;
        ofs << std::endl;
  
      }
  
    } else {
  
      for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
      
        CameraPosition cameraPosition = image->cameraPosition();
        if (cameraPosition.isEmpty()) {
          id++;
          continue; /// Se saltan las imagenes no orientadas
        }
  
        math::Quaternion<double> quaternion = cameraPosition.quaternion();
        std::string file_name = image->path().toStdString(); //Path(image->path().toStdString()).fileName();
        tl::Point3D position(cameraPosition.x(), cameraPosition.y(), cameraPosition.z());
  
        if (!bLocalCoord) {
          position -= offset;
        }
  
        math::Vector<double, 3> vector_camera_position = position.vector();
  
        math::RotationMatrix<double> r_ip_ic = math::RotationMatrix<double>::identity();
        r_ip_ic.at(1, 1) = -1;
        r_ip_ic.at(2, 2) = -1;
  
        math::RotationMatrix<double> rotation_matrix;
        math::RotationConverter<double>::convert(quaternion, rotation_matrix);
  
        math::RotationMatrix<double> rotation = r_ip_ic * rotation_matrix.transpose();
        tl::math::RotationConverter<double>::convert(rotation, quaternion);
        quaternion.normalize();
  
        vector_camera_position = rotation * -vector_camera_position;
  
        ofs << std::fixed << id++ << " " << quaternion.w << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << " " <<
          vector_camera_position[0] << " " << vector_camera_position[1] << " " << vector_camera_position[2] << " 1 " << file_name << std::endl;
        ofs << std::endl;
  
      }
    }
  
    ofs.close();
  }
  
  /// Escritura de fichero cameras.txt
  
  {
    tl::Path cameras_path(input_path);
    cameras_path.append("cameras.txt");
  
    std::ofstream ofs;
    ofs.open(cameras_path.toString(), std::ofstream::out | std::ofstream::trunc);
  
    if (!ofs.is_open()) return 1;
  
    ofs << "# Camera list with one line of data per camera: \n";
    ofs << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]\n";
    ofs << "# Number of cameras: " << project.camerasCount() << "\n";
  
    for (auto it = project.cameraBegin(); it != project.cameraEnd(); it++) {
      Camera camera = it->second;
      std::string camera_type = cameraToColmapType(camera).toStdString();
      std::shared_ptr<Calibration> calibration = camera.calibration();
      
      if (calibration) {
  
        double focal = calibration->existParameter(Calibration::Parameters::focal) ? calibration->parameter(Calibration::Parameters::focal) : std::min(camera.width(), camera.height());
        double cx = calibration->existParameter(Calibration::Parameters::cx) ? calibration->parameter(Calibration::Parameters::cx) : camera.width() / 2.;
        double cy = calibration->existParameter(Calibration::Parameters::cy) ? calibration->parameter(Calibration::Parameters::cy) : camera.height() / 2.;
        double k1 = calibration->existParameter(Calibration::Parameters::k1) ? calibration->parameter(Calibration::Parameters::k1) : 0.0;
  
        ofs << it->first << " " << camera_type << " " << camera.width() << " " << camera.height() << " "
          << focal << " " << cx << " " << cy << " " << k1;
  
        if (camera_type.compare("RADIAL") == 0 || camera_type.compare("FULL_RADIAL") == 0) {
          double k2 = calibration->existParameter(Calibration::Parameters::k2) ? calibration->parameter(Calibration::Parameters::k2) : 0.0;
          ofs << " " << k2;
        }
  
        if (camera_type.compare("FULL_RADIAL") == 0) {
          double p1 = calibration->existParameter(Calibration::Parameters::p1) ? calibration->parameter(Calibration::Parameters::p1) : 0.0;
          double p2 = calibration->existParameter(Calibration::Parameters::p2) ? calibration->parameter(Calibration::Parameters::p2) : 0.0;
          ofs << " " << p1 << " " << p2;
        }
  
      } else {
  
        double focal = std::min(camera.width(), camera.height());
        double cx = camera.width() / 2.;
        double cy = camera.height() / 2.;
        double k1 = 0.0;
  
        ofs << it->first << " " << camera_type << " " << camera.width() << " " << camera.height() << " "
          << focal << " " << cx << " " << cy << " " << k1;
  
        if (camera_type.compare("RADIAL") == 0 || camera_type.compare("FULL_RADIAL") == 0) {
          double k2 = 0.0;
          ofs << " " << k2;
        }
  
        if (camera_type.compare("FULL_RADIAL") == 0) {
          double p1 = 0.0;
          double p2 = 0.0;
          ofs << " " << p1 << " " << p2;
        }
  
      }
  
      ofs << std::endl;
    }
  
  
    ofs.close();
  }
  
  {
  
    tl::Path points3d_path(input_path);
    points3d_path.append("points3D.txt");
  
    std::ofstream ofs;
    ofs.open(points3d_path.toString(), std::ofstream::out | std::ofstream::trunc);
    ofs.close();
  }

  std::string reconstruction_path = project.projectFolder().toStdString();

  if (bLocalCoord) {
    reconstruction_path.append("/ori/relative/");
  } else {
    reconstruction_path.append("/ori/absolute/");
  }
  Path::createDirectories(reconstruction_path);

  bool clear_points = false;

  colmap::OptionManager options;
  options.AddDatabaseOptions();
  options.AddImageOptions();
  options.AddRequiredOption("input_path", &input_path.toString());
  options.AddRequiredOption("reconstruction_path", &reconstruction_path);
  options.AddDefaultOption(
    "clear_points", &clear_points,
    "Whether to clear all existing points and observations");
  options.AddMapperOptions();
  //options.Parse(argc, argv);

  //if (!QFileInfo(input_path.c_str()).exists()) {
  //  std::cerr << "ERROR: `input_path` is not a directory" << std::endl;
  //  return EXIT_FAILURE;
  //}

  if (!QFileInfo(reconstruction_path.c_str()).exists()) {
    std::cerr << "ERROR: `reconstruction_path` is not a directory" << std::endl;
    return EXIT_FAILURE;
  }

  const auto &mapper_options = *options.mapper;

  msgInfo("Loading model");

  colmap::Reconstruction reconstruction;
  reconstruction.Read(input_path.toString());
  //colmap::Reconstruction reconstruction2;
  //reconstruction2.Read(input_path.toString());

  msgInfo("Loading database");

  colmap::DatabaseCache database_cache;

  {
    Chrono timer("Elapsed time:");
    timer.run();
    
    colmap::Database database(project.database().toStdString());

    const size_t min_num_matches =
      static_cast<size_t>(mapper_options.min_num_matches);
    database_cache.Load(database, min_num_matches,
                        mapper_options.ignore_watermarks,
                        mapper_options.image_names);

    if (clear_points) {
      reconstruction.DeleteAllPoints2DAndPoints3D();
      reconstruction.TranscribeImageIdsToDatabase(database);
    }

    std::cout << std::endl;
    timer.stop();
  }

  std::cout << std::endl;

  CHECK_GE(reconstruction.NumRegImages(), 2)
    << "Need at least two images for triangulation";

  colmap::IncrementalMapper mapper(&database_cache);
  mapper.BeginReconstruction(&reconstruction);

  //////////////////////////////////////////////////////////////////////////////
  // Triangulation
  //////////////////////////////////////////////////////////////////////////////

  const auto tri_options = mapper_options.Triangulation();

  const auto &reg_image_ids = reconstruction.RegImageIds();

  for (size_t i = 0; i < reg_image_ids.size(); ++i) {
    const colmap::image_t image_id = reg_image_ids[i];

    const auto &image = reconstruction.Image(image_id);

    //PrintHeading1(StringPrintf("Triangulating image #%d (%d)", image_id, i));

    const size_t num_existing_points3D = image.NumPoints3D();

    std::cout << "  => Image sees " << num_existing_points3D << " / "
      << image.NumObservations() << " points" << std::endl;

    mapper.TriangulateImage(tri_options, image_id);

    std::cout << "  => Triangulated "
      << (image.NumPoints3D() - num_existing_points3D) << " points"
      << std::endl;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Retriangulation
  //////////////////////////////////////////////////////////////////////////////

  msgInfo("Retriangulation");

  CompleteAndMergeTracks(mapper_options, &mapper);

  //////////////////////////////////////////////////////////////////////////////
  // Bundle adjustment
  //////////////////////////////////////////////////////////////////////////////

  auto ba_options = mapper_options.GlobalBundleAdjustment();

  ba_options.refine_focal_length = !fix_calibration;
  ba_options.refine_principal_point = false;
  ba_options.refine_extra_params = !fix_calibration;
  ba_options.refine_extrinsics = false;

  // Configure bundle adjustment.
  colmap::BundleAdjustmentConfig ba_config;
  for (const colmap::image_t image_id : reconstruction.RegImageIds()) {
    ba_config.AddImage(image_id);
  }

  for (int i = 0; i < mapper_options.ba_global_max_refinements; ++i) {
    // Avoid degeneracies in bundle adjustment.
    reconstruction.FilterObservationsWithNegativeDepth();

    const size_t num_observations = reconstruction.ComputeNumObservations();

    //PrintHeading1("Bundle adjustment");
    //inspector::BundleAdjuster bundle_adjuster(ba_options, ba_config);
    colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
    CHECK(bundle_adjuster.Solve(&reconstruction));

    size_t num_changed_observations = 0;
    num_changed_observations += CompleteAndMergeTracks(mapper_options, &mapper);
    num_changed_observations += FilterPoints(mapper_options, &mapper);
    const double changed =
      static_cast<double>(num_changed_observations) / num_observations;
    std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed)
      << std::endl;
    if (changed < mapper_options.ba_global_max_refinement_change) {
      break;
    }
  }

  // Se incluye el punto principal en el ajuste
  if (!fix_calibration) {
    ba_options.refine_principal_point = true;
    for (int i = 0; i < mapper_options.ba_global_max_refinements; ++i) {
      // Avoid degeneracies in bundle adjustment.
      reconstruction.FilterObservationsWithNegativeDepth();

      const size_t num_observations = reconstruction.ComputeNumObservations();

      //PrintHeading1("Bundle adjustment");
      //inspector::BundleAdjuster bundle_adjuster(ba_options, ba_config);
      colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
      CHECK(bundle_adjuster.Solve(&reconstruction));

      size_t num_changed_observations = 0;
      num_changed_observations += CompleteAndMergeTracks(mapper_options, &mapper);
      num_changed_observations += FilterPoints(mapper_options, &mapper);
      const double changed =
        static_cast<double>(num_changed_observations) / num_observations;
      std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed)
        << std::endl;
      if (changed < mapper_options.ba_global_max_refinement_change) {
        break;
      }
    }
  }
  

  //for (const colmap::image_t image_id : reconstruction.RegImageIds()) {
  //  colmap::Image &image = reconstruction.Image(image_id);
  //  image.SetQvec(reconstruction2.Image(image_id).Qvec());
  //  image.SetTvec(reconstruction2.Image(image_id).Tvec());
  //}

  //ba_options.refine_focal_length = false;
  //ba_options.refine_principal_point = false;
  //ba_options.refine_extra_params = false;
  //ba_options.refine_extrinsics = false;

  //for (int i = 0; i < mapper_options.ba_global_max_refinements; ++i) {
  //  // Avoid degeneracies in bundle adjustment.
  //  reconstruction.FilterObservationsWithNegativeDepth();

  //  const size_t num_observations = reconstruction.ComputeNumObservations();

  //  //PrintHeading1("Bundle adjustment");
  //  //inspector::BundleAdjuster bundle_adjuster(ba_options, ba_config);
  //  colmap::BundleAdjuster bundle_adjuster(ba_options, ba_config);
  //  CHECK(bundle_adjuster.Solve(&reconstruction));

  //  size_t num_changed_observations = 0;
  //  num_changed_observations += CompleteAndMergeTracks(mapper_options, &mapper);
  //  num_changed_observations += FilterPoints(mapper_options, &mapper);
  //  const double changed =
  //    static_cast<double>(num_changed_observations) / num_observations;
  //  std::cout << colmap::StringPrintf("  => Changed observations: %.6f", changed)
  //    << std::endl;
  //  if (changed < mapper_options.ba_global_max_refinement_change) {
  //    break;
  //  }
  //}

  msgInfo("Extracting colors");
  reconstruction.ExtractColorsForAllImages(""/*project.imageDirectory().toStdString()*/);

  const bool kDiscardReconstruction = false;
  mapper.EndReconstruction(kDiscardReconstruction);

  reconstruction.Write(reconstruction_path);
  reconstruction.WriteText(reconstruction_path);

  OrientationExport orientationExport(&reconstruction);
  std::string sparse_model = reconstruction_path + "/sparse.ply";
  orientationExport.exportPLY(sparse_model.c_str());

  std::string offset_file{};

  if (!bLocalCoord) {

    offset_file = reconstruction_path + "/offset.txt";

    std::ofstream ofs;
    ofs.open(offset_file, std::ofstream::out | std::ofstream::trunc);
    
    ofs << QString::number(offset.x, 'f', 3).toStdString() << " "
        << QString::number(offset.y, 'f', 3).toStdString() << " "
        << QString::number(offset.z, 'f', 3).toStdString() << std::endl;

    msgInfo("Camera offset: %lf,%lf,%lf", offset.x, offset.y, offset.z);


    ofs.close();
  }
    
  project.setReconstructionPath(reconstruction_path.c_str());
  project.setSparseModel(sparse_model.c_str());
  project.setOffset(offset_file.c_str());

  ReadPhotoOrientations readPhotoOrientations;
  readPhotoOrientations.open(reconstruction_path.c_str());
  int oriented_images = 0;

  for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {
    QString image_oriented = image->path(); //QFileInfo(image->path()).fileName();
    CameraPose photoOrientation = readPhotoOrientations.orientation(QFileInfo(image->path()).fileName());
    if (photoOrientation.position.x != 0. && photoOrientation.position.y != 0. && photoOrientation.position.z != 0.) {
      project.addPhotoOrientation(image->name(), photoOrientation);
      oriented_images++;
    } else {
      QByteArray ba = image_oriented.toLocal8Bit();
      const char *msg = ba.constData();
      msgWarning("Image %s not oriented", msg);
    }
  }

  msgInfo("Oriented %i images", oriented_images);

  ReadCalibration readCalibration;
  readCalibration.open(reconstruction_path.c_str());
  std::shared_ptr<Calibration> calibration;
  for (auto camera_it = project.cameraBegin(); camera_it != project.cameraEnd(); camera_it++) {
    calibration = readCalibration.calibration(camera_it->first);
    if (calibration) {
      Camera camera = camera_it->second;
      camera.setCalibration(calibration);
      project.updateCamera(camera_it->first, camera);
    }
  }

  project.save(project_file.toString().c_str());

  return 0;
}
