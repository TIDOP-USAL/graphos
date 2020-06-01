#include <inspector/process/features/FeatureMatchingProcess.h>

#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/features/matching.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>

#include <QDir>
#include <QTextStream>

using namespace inspector;
using namespace tl;

int main(int argc, char** argv)
{

  tl::Chrono chrono("Feature Matching");
  chrono.run();

/* Configuración de consola */

  std::string cmd_description = "Feature Matching";

  tl::Console &console = tl::Console::instance();
  console.setLogLevel(tl::MessageLevel::msg_verbose);
  console.setTitle(cmd_description);
  tl::MessageManager::instance().addListener(&console);

  TL_TODO("Esto se tiene que leer de settings para establecer los valores por defecto")
  std::shared_ptr<FeatureMatchingProperties> featureMatchingProperties(new FeatureMatchingProperties);

/* Parseo de argumentos */

  std::string project_file;
  //int block_size = TL_INT_MIN;
  double ratio = featureMatchingProperties->ratio();
  double distance = featureMatchingProperties->distance();
  double max_error = featureMatchingProperties->maxError();
  double confidence = featureMatchingProperties->confidence();
  //int maxMatches = TL_INT_MIN;
  bool cross_check = featureMatchingProperties->crossCheck();

  Command cmd("featmatch", cmd_description);
  cmd.push_back(std::make_shared<ArgumentStringRequired>("prj", 'p', "Project file", &project_file));
  //cmd.push_back(std::make_shared<ArgumentIntegerOptional>("block_size", 'b', "Number of images to simultaneously load into memory (default = 50)", &block_size));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("ratio", "Ratio (default = 0.8)", &ratio));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("distance", "Distance (default = 0.7)", &distance));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("max_error", "Maximun error (default = 4.0)", &max_error));
  cmd.push_back(std::make_shared<ArgumentDoubleOptional>("confidence", "Confidence (default = 0.999)", &confidence));
  //cmd.push_back(std::make_shared<ArgumentIntegerOptional>("max_matches", "Maximun Matches (default = 32768)", &maxMatches));
  cmd.push_back(std::make_shared<ArgumentBooleanOptional>("cross_check", "If true, cross checking is enabled (default = true)", &cross_check));

  cmd.addExample("featmatch --p 253/253.xml");

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

  if (QFileInfo(project_file.c_str()).exists()){

  } else {
    msgError("The project doesn't exist");
    return 1;
  }

  ProjectImp project;
  project.load(project_file.c_str());

  QString database_path = project.database();
  QString project_dir = project.projectFolder();

  QDir dir(project_dir);

/* Fichero log */

  QString log_file = project_dir + "/" + project.name() + ".log";

  tl::Log &log = tl::Log::instance();
  log.setLogLevel(tl::MessageLevel::msg_verbose);
  log.setLogFile(log_file.toLatin1());
  tl::MessageManager::instance().addListener(&log);

/* Chequeo de soporte CUDA */

  bool bUseGPU = cudaEnabled(8.0, 5.0);

  try {
    colmap::ExhaustiveMatchingOptions exhaustiveMatchingOptions;
    colmap::SiftMatchingOptions siftMatchingOptions;
    siftMatchingOptions.max_error = max_error;
    siftMatchingOptions.cross_check = cross_check;
    siftMatchingOptions.max_ratio = ratio;
    siftMatchingOptions.max_distance = distance;
    siftMatchingOptions.confidence = confidence;
  //  siftMatchingOptions.max_num_matches = ;
    siftMatchingOptions.use_gpu = bUseGPU;

    colmap::ExhaustiveFeatureMatcher exhaustiveFeatureMatcher(exhaustiveMatchingOptions,
                                                              siftMatchingOptions,
                                                              database_path.toStdString());

    exhaustiveFeatureMatcher.Start();
    exhaustiveFeatureMatcher.Wait();

    colmap::Database database(database_path.toStdString());
    if (database.NumMatches() == 0) {
      msgError("Matching error");
      return 1;
    }

    featureMatchingProperties->setRatio(ratio);
    featureMatchingProperties->setDistance(distance);
    featureMatchingProperties->setMaxError(max_error);
    featureMatchingProperties->setConfidence(confidence);
    featureMatchingProperties->enableCrossCheck(cross_check);
    project.setFeatureMatching(featureMatchingProperties);
    project.save(project_file.c_str());
    msgInfo("Project saved");

    uint64_t time = chrono.stop();
    msgInfo("[Time: %f seconds]", time/1000.);

  } catch (std::exception &e) {
    msgError(e.what());
  }

  return 0;
}
