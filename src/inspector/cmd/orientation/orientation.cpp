#include <inspector/core/utils.h>
#include <inspector/core/project.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>
#include <tidop/core/chrono.h>
#include <tidop/core/path.h>
//#include <tidop/geospatial/crs.h>
//#include <tidop/geospatial/crstransf.h>
//#include <tidop/geometry/entities/point.h>
//#include <tidop/math/algebra/euler_angles.h>
#include <tidop/math/algebra/quaternion.h>
//#include <tidop/math/algebra/rotation_convert.h>
//#include <tidop/math/math.h>

#include <colmap/util/option_manager.h>
#include <colmap/base/reconstruction.h>
#include <colmap/base/database.h>
#include <colmap/base/database_cache.h>
#include <colmap/controllers/incremental_mapper.h>

#include <QDir>
#include <QTextStream>

using namespace inspector;
using namespace tl;
//using namespace geospatial;



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

  Command cmd("orientation", cmd_description);
  cmd.push_back(std::make_shared<ArgumentPathRequired>("prj", 'p', "Project file", &project_file));

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
  tl::Path input_path = tl::Path::tempDirectory(); // Tiene que ser un directorio temporal en el que se exporte la reconstrucción como texto.
  input_path.append("import_ori");
  Path::createDirectory(input_path);

  /// leer imagenes y escribir images.txt en reconstrucción temporal
  {
    tl::Path images_path(input_path);
    images_path.append("images.txt");

    std::ofstream ofs;
    ofs.open(images_path.toString(), std::ofstream::out | std::ofstream::trunc);

    if (!ofs.is_open()) return 1;

    size_t id = 1;
    for (auto image = project.imageBegin(); image != project.imageEnd(); image++) {

      std::string file_name = Path(image->path().toStdString()).fileName();

      CameraPosition cameraPosition = image->cameraPosition();
      math::Quaternion<double> quaternion = cameraPosition.quaternion();

      ofs << std::fixed << id++ << " " << quaternion.w << " " << quaternion.x << " " << quaternion.y << " " << quaternion.z << " " <<
        cameraPosition.x() << " " << cameraPosition.y() << " " << cameraPosition.z() << " 1 " << file_name << std::endl;
      ofs << std::endl;
    }

    ofs.close();
  }
  
  /// Escritura de fichero cameras.txt
  /// Ver si hay calibración o se añade calibración por defecto
  bool bNoCalibration = true;
  
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

        bNoCalibration = false;

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





  std::string reconstruction_path = project.reconstructionPath().toStdString();
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
  ba_options.refine_focal_length = bNoCalibration ? true : false;
  ba_options.refine_principal_point = false;
  ba_options.refine_extra_params = bNoCalibration ? true : false;
  ba_options.refine_extrinsics = false;

  // Configure bundle adjustment.
  colmap::BundleAdjustmentConfig ba_config;
  for (const colmap::image_t image_id : reconstruction.RegImageIds()) {
    ba_config.AddImage(image_id);
    //ba_config.SetVariablePose(image_id);
    //ba_config.SetConstantTvec(image_id, {0});
    //ba_config.SetConstantTvec(image_id, {1});
    //ba_config.SetConstantTvec(image_id, {2});
    //ba_config.SetConstantTvec(image_id, {0, 1, 2 }); // Asi da un error
  }

  for (int i = 0; i < mapper_options.ba_global_max_refinements; ++i) {
    // Avoid degeneracies in bundle adjustment.
    reconstruction.FilterObservationsWithNegativeDepth();

    const size_t num_observations = reconstruction.ComputeNumObservations();

    //PrintHeading1("Bundle adjustment");
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

  msgInfo("Extracting colors");
  reconstruction.ExtractColorsForAllImages(project.imageDirectory().toStdString());

  const bool kDiscardReconstruction = false;
  mapper.EndReconstruction(kDiscardReconstruction);

  reconstruction.Write(reconstruction_path);

  return 0;
}
