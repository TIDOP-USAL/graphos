#include <inspector/process/features/FeatureMatchingProcess.h>

#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/features/matching.h>
#include <inspector/core/orientation/orientationexport.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/base/reconstruction.h>

#include <QDir>
#include <QTextStream>

using namespace inspector;
using namespace tl;

int main(int argc, char** argv)
{

/* Configuración de consola */

  std::string cmd_description = "Feature Matching";

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

  TL_TODO("Esto se tiene que leer de settings para establecer los valores por defecto")
  std::shared_ptr<FeatureMatchingProperties> featureMatchingProperties(new FeatureMatchingProperties);

/* Parseo de argumentos */

  std::string reconstruction_path;
  std::string ply_file;
  ////int block_size = TL_INT_MIN;
  //double ratio = featureMatchingProperties->ratio();
  //double distance = featureMatchingProperties->distance();
  //double max_error = featureMatchingProperties->maxError();
  //double confidence = featureMatchingProperties->confidence();
  ////int maxMatches = TL_INT_MIN;
  //bool cross_check = featureMatchingProperties->crossCheck();

  Command cmd("featmatch", cmd_description);
  cmd.push_back(std::make_shared<ArgumentStringRequired>("reconstruction_path", 'r', "reconstruction path", &reconstruction_path));
  cmd.push_back(std::make_shared<ArgumentStringRequired>("ply", 'p', "ply file", &ply_file));
  //cmd.push_back(std::make_shared<ArgumentDoubleOptional>("ratio", "Ratio (default = 0.8)", &ratio));
  //cmd.push_back(std::make_shared<ArgumentDoubleOptional>("distance", "Distance (default = 0.7)", &distance));
  //cmd.push_back(std::make_shared<ArgumentDoubleOptional>("max_error", "Maximun error (default = 4.0)", &max_error));
  //cmd.push_back(std::make_shared<ArgumentDoubleOptional>("confidence", "Confidence (default = 0.999)", &confidence));
  //cmd.push_back(std::make_shared<ArgumentBooleanOptional>("cross_check", "If true, cross checking is enabled (default = true)", &cross_check));

  //cmd.addExample("featmatch --p 253/253.xml");

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

  //if (QFileInfo(project_file.c_str()).exists()){

  //} else {
  //  msgError("The project doesn't exist");
  //  return 1;
  //}

  //ProjectImp project;
  //project.load(project_file.c_str());

  //QString database_path = project.database();
  //QString project_dir = project.projectFolder();

  //QDir dir(project_dir);

/* Fichero log */

  //QString log_file = project_dir + "/" + project.name() + ".log";

  //tl::Log &log = tl::Log::instance();
  //log.setLogLevel(tl::MessageLevel::msg_verbose);
  //log.setLogFile(log_file.toLatin1());
  //tl::MessageManager::instance().addListener(&log);

/* Chequeo de soporte CUDA */

  //bool bUseGPU = cudaEnabled(8.0, 5.0);

  colmap::Reconstruction reconstruction;


  try {

#ifdef _DEBUG
    //Lectura como texto
    reconstruction.ReadText(reconstruction_path);
#else
    //Lectura como binario
    reconstruction.ReadBinary(reconstruction_path);
#endif
 
    OrientationExport orientationExport(&reconstruction);
    orientationExport.exportPLY(ply_file.c_str());
    //for(auto &image : reconstruction.Images()){

    //  const Eigen::Matrix<float, 3, 4> inv_proj_matrix = image.second.InverseProjectionMatrix().cast<float>();
    //  const Eigen::Vector3f pc = inv_proj_matrix.rightCols<1>();
    //  std::string name = image.second.Name();
    //  msgInfo("%s: [%f,%f,%f]", name.c_str(), pc(0), pc(1), pc(2));

    //  const Eigen::Matrix<double, 3, 4> inv_proj_matrix_d = image.second.InverseProjectionMatrix();
    //  const Eigen::Vector3d pcd = inv_proj_matrix_d.rightCols<1>();
    //  msgInfo("%s: [%lf,%lf,%lf]", name.c_str(), pcd(0), pcd(1), pcd(2));
    //}

    //for (auto &point : reconstruction.Points3D()) {
    //  colmap::point3D_t id = point.first;
    //  msgInfo("%i: [%lf,%lf,%lf]", static_cast<int>(id), point.second.X(), point.second.Y(), point.second.Z());
    //}

  //  colmap::ExhaustiveMatchingOptions exhaustiveMatchingOptions;
  //  colmap::SiftMatchingOptions siftMatchingOptions;
  //  siftMatchingOptions.max_error = max_error;
  //  siftMatchingOptions.cross_check = cross_check;
  //  siftMatchingOptions.max_ratio = ratio;
  //  siftMatchingOptions.max_distance = distance;
  //  siftMatchingOptions.confidence = confidence;
  ////  siftMatchingOptions.max_num_matches = ;
  //  siftMatchingOptions.use_gpu = bUseGPU;

  //  colmap::ExhaustiveFeatureMatcher exhaustiveFeatureMatcher(exhaustiveMatchingOptions,
  //                                                            siftMatchingOptions,
  //                                                            database_path.toStdString());

  //  exhaustiveFeatureMatcher.Start();
  //  exhaustiveFeatureMatcher.Wait();

  //  colmap::Database database(database_path.toStdString());
  //  if (database.NumMatches() == 0) {
  //    msgError("Matching error");
  //    return 1;
  //  }

  //  featureMatchingProperties->setRatio(ratio);
  //  featureMatchingProperties->setDistance(distance);
  //  featureMatchingProperties->setMaxError(max_error);
  //  featureMatchingProperties->setConfidence(confidence);
  //  featureMatchingProperties->enableCrossCheck(cross_check);
  //  project.setFeatureMatching(featureMatchingProperties);
  //  project.save(project_file.c_str());
  //  msgInfo("Project saved");


  } catch (std::exception &e) {
    msgError(e.what());
  }

  return 0;
}
