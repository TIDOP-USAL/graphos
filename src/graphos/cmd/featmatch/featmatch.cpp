#include <inspector/process/features/FeatureMatchingProcess.h>

#include <inspector/core/utils.h>
#include <inspector/core/project.h>
#include <inspector/core/features/matching.h>

#include <tidop/core/utils.h>
#include <tidop/core/messages.h>
#include <tidop/core/console.h>
#include <tidop/core/log.h>
#include <tidop/core/chrono.h>

#include <colmap/util/option_manager.h>
#include <colmap/util/misc.h>
#include <colmap/feature/sift.h>
#include <colmap/feature/matching.h>

#include <QDir>
#include <QTextStream>



/**/
#include "util/cuda.h"

#include <unordered_set>
/**/

using namespace inspector;
using namespace tl;

void PrintElapsedTime(const colmap::Timer& timer) {
  std::cout << colmap::StringPrintf(" in %.3fs", timer.ElapsedSeconds()) << std::endl;
}

std::vector<Eigen::Vector2d> FeatureKeypointsToPointsVector(
    const colmap::FeatureKeypoints& keypoints) {
  std::vector<Eigen::Vector2d> points(keypoints.size());
  for (size_t i = 0; i < keypoints.size(); ++i) {
    points[i] = Eigen::Vector2d(keypoints[i].x, keypoints[i].y);
  }
  return points;
}


class TwoViewGeometryVerifier : public colmap::Thread {
 public:
  typedef colmap::internal::FeatureMatcherData Input;
  typedef colmap::internal::FeatureMatcherData Output;

  TwoViewGeometryVerifier(const colmap::SiftMatchingOptions& options,
                          colmap::FeatureMatcherCache* cache,
                          colmap::JobQueue<Input>* input_queue,
                          colmap::JobQueue<Output>* output_queue);
  ~TwoViewGeometryVerifier()
  {
  }

 protected:
  void Run() override;

  const colmap::SiftMatchingOptions options_;
  colmap::TwoViewGeometry::Options two_view_geometry_options_;
  colmap::FeatureMatcherCache* cache_;
  colmap::JobQueue<Input>* input_queue_;
  colmap::JobQueue<Output>* output_queue_;
};

TwoViewGeometryVerifier::TwoViewGeometryVerifier(
    const colmap::SiftMatchingOptions& options, colmap::FeatureMatcherCache* cache,
    colmap::JobQueue<Input>* input_queue, colmap::JobQueue<Output>* output_queue)
    : options_(options),
      cache_(cache),
      input_queue_(input_queue),
      output_queue_(output_queue) {
  CHECK(options_.Check());

  two_view_geometry_options_.min_num_inliers =
      static_cast<size_t>(options_.min_num_inliers);
  two_view_geometry_options_.ransac_options.max_error = options_.max_error;
  two_view_geometry_options_.ransac_options.confidence = options_.confidence;
  two_view_geometry_options_.ransac_options.min_num_trials =
      static_cast<size_t>(options_.min_num_trials);
  two_view_geometry_options_.ransac_options.max_num_trials =
      static_cast<size_t>(options_.max_num_trials);
  two_view_geometry_options_.ransac_options.min_inlier_ratio =
      options_.min_inlier_ratio;

}

void TwoViewGeometryVerifier::Run() {
  
  while (true) {
    if (Thread::IsStopped()) {
      break;
    }
    const auto input_job = input_queue_->Pop();
    if (input_job.IsValid()) {
      auto data = input_job.Data();

      if (data.matches.size() < static_cast<size_t>(options_.min_num_inliers)) {
        CHECK(output_queue_->Push(data));
        continue;
      }

      const auto& camera1 =
          cache_->GetCamera(cache_->GetImage(data.image_id1).CameraId());
      const auto& camera2 =
          cache_->GetCamera(cache_->GetImage(data.image_id2).CameraId());
      const auto keypoints1 = cache_->GetKeypoints(data.image_id1);
      const auto keypoints2 = cache_->GetKeypoints(data.image_id2);
      const auto points1 = FeatureKeypointsToPointsVector(keypoints1);
      const auto points2 = FeatureKeypointsToPointsVector(keypoints2);

      if (options_.multiple_models) {
        data.two_view_geometry.EstimateMultiple(camera1, points1, camera2,
                                                points2, data.matches,
                                                two_view_geometry_options_);
      } else {
        data.two_view_geometry.Estimate(camera1, points1, camera2, points2,
                                        data.matches,
                                        two_view_geometry_options_);
      }

      CHECK(output_queue_->Push(data));
    }
  }
}

int main(int argc, char** argv)
{

  tl::Chrono chrono("Feature Matching");
  chrono.run();

/* Configuración de consola */

  std::string cmd_description = "Feature Matching";

  tl::Console &console = tl::Console::instance();
  console.setMessageLevel(tl::MessageLevel::msg_verbose);
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

  //QString log_file = project_dir + "/" + project.name() + ".log";

  //tl::Log &log = tl::Log::instance();
  //log.setMessageLevel(tl::MessageLevel::msg_verbose);
  //log.setLogFile(log_file.toStdString());
  //tl::MessageManager::instance().addListener(&log);

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
    //siftMatchingOptions.max_num_matches = 10000;
    siftMatchingOptions.use_gpu = bUseGPU;

    //colmap::ExhaustiveFeatureMatcher exhaustiveFeatureMatcher(exhaustiveMatchingOptions,
    //                                                          siftMatchingOptions,
    //                                                          database_path.toStdString());

    //exhaustiveFeatureMatcher.Start();
    //exhaustiveFeatureMatcher.Wait();
    {
      colmap::ExhaustiveMatchingOptions options_ = exhaustiveMatchingOptions;
      colmap::SiftMatchingOptions match_options_ = siftMatchingOptions;
      colmap::Database database_(database_path.toStdString());
      colmap::FeatureMatcherCache cache_(5 * options_.block_size, &database_);
      //colmap::SiftFeatureMatcher matcher_(siftMatchingOptions, &database_, &cache_);
      std::vector<std::unique_ptr<colmap::FeatureMatcherThread>> matchers_;
      colmap::JobQueue<colmap::internal::FeatureMatcherData> matcher_queue_;
      colmap::JobQueue<colmap::internal::FeatureMatcherData> verifier_queue_;
      std::vector<std::unique_ptr<colmap::Thread>> verifiers_;
      colmap::JobQueue<colmap::internal::FeatureMatcherData> output_queue_;
      {
        const int num_threads = colmap::GetEffectiveNumThreads(match_options_.num_threads);
        //CHECK_GT(num_threads, 0);

        std::vector<int> gpu_indices = colmap::CSVToVector<int>(match_options_.gpu_index);
        //CHECK_GT(gpu_indices.size(), 0);

#ifdef HAVE_CUDA //CUDA_ENABLED
        if (match_options_.use_gpu && gpu_indices.size() == 1 && gpu_indices[0] == -1) {
          const int num_cuda_devices = colmap::GetNumCudaDevices();
          CHECK_GT(num_cuda_devices, 0);
          gpu_indices.resize(num_cuda_devices);
          std::iota(gpu_indices.begin(), gpu_indices.end(), 0);
        }
#endif  // CUDA_ENABLED

        if (match_options_.use_gpu) {
          auto gpu_options = match_options_;
          matchers_.reserve(gpu_indices.size());
          for (const auto &gpu_index : gpu_indices) {
            gpu_options.gpu_index = std::to_string(gpu_index);
            matchers_.emplace_back(new colmap::SiftGPUFeatureMatcher(
              gpu_options, &cache_, &matcher_queue_, &verifier_queue_));
          }
        } else {
          matchers_.reserve(num_threads);
          for (int i = 0; i < num_threads; ++i) {
            matchers_.emplace_back(new colmap::SiftCPUFeatureMatcher(
              match_options_, &cache_, &matcher_queue_, &verifier_queue_));
          }
        }

        verifiers_.reserve(num_threads);

        for (int i = 0; i < num_threads; ++i) {
          verifiers_.emplace_back(new TwoViewGeometryVerifier(
            match_options_, &cache_, &verifier_queue_, &output_queue_));
        }
      }

      colmap::PrintHeading1("Exhaustive feature matching");

      /****************************************************************************/
      //if (!matcher_.Setup()) {
      //  return 1;
      //}
      const int max_num_features = database_.MaxNumDescriptors();
      match_options_.max_num_matches =
          std::min(match_options_.max_num_matches, max_num_features);
  
      for (auto& matcher : matchers_) {
        matcher->SetMaxNumMatches(match_options_.max_num_matches);
        matcher->Start();
      }

      for (auto& verifier : verifiers_) {
        verifier->Start();
      }
  
      for (auto &matcher : matchers_) {
        if (!matcher->CheckValidSetup()) {
          return 1;
        }
      }
      /**********************************************************************************/
      cache_.Setup();

      const std::vector<colmap::image_t> image_ids = cache_.GetImageIds();

      const size_t block_size = static_cast<size_t>(options_.block_size);
      const size_t num_blocks = static_cast<size_t>(
        std::ceil(static_cast<double>(image_ids.size()) / block_size));
      const size_t num_pairs_per_block = block_size * (block_size - 1) / 2;

      std::vector<std::pair<colmap::image_t, colmap::image_t>> image_pairs;
      image_pairs.reserve(num_pairs_per_block);

      for (size_t start_idx1 = 0; start_idx1 < image_ids.size();
           start_idx1 += block_size) {
        const size_t end_idx1 =
          std::min(image_ids.size(), start_idx1 + block_size) - 1;
        for (size_t start_idx2 = 0; start_idx2 < image_ids.size();
             start_idx2 += block_size) {
          const size_t end_idx2 =
            std::min(image_ids.size(), start_idx2 + block_size) - 1;

          //if (IsStopped()) {
          //  GetTimer().PrintMinutes();
          //  return;
          //}

          colmap::Timer timer;
          timer.Start();

          std::cout << colmap::StringPrintf("Matching block [%d/%d, %d/%d]",
                                    start_idx1 / block_size + 1, num_blocks,
                                    start_idx2 / block_size + 1, num_blocks)
            << std::flush;

          image_pairs.clear();
          for (size_t idx1 = start_idx1; idx1 <= end_idx1; ++idx1) {
            for (size_t idx2 = start_idx2; idx2 <= end_idx2; ++idx2) {
              const size_t block_id1 = idx1 % block_size;
              const size_t block_id2 = idx2 % block_size;
              if ((idx1 > idx2 && block_id1 <= block_id2) ||
                  (idx1 < idx2 &&
                  block_id1 < block_id2)) {  // Avoid duplicate pairs
                image_pairs.emplace_back(image_ids[idx1], image_ids[idx2]);
              }
            }
          }

          colmap::DatabaseTransaction database_transaction(&database_);
          /***********************************************************************************/
          //matcher_.Match(image_pairs);

          if (image_pairs.empty()) {
            return 1;
          }
  
          if (image_pairs.empty()) {
            return 1;
          }

          //////////////////////////////////////////////////////////////////////////////
          // Match the image pairs
          //////////////////////////////////////////////////////////////////////////////

          std::unordered_set<colmap::image_pair_t> image_pair_ids;
          image_pair_ids.reserve(image_pairs.size());

          size_t num_outputs = 0;
          for (const auto image_pair : image_pairs) {
            // Avoid self-matches.
            if (image_pair.first == image_pair.second) {
              continue;
            }

            // Avoid duplicate image pairs.
            const colmap::image_pair_t pair_id =
              colmap::Database::ImagePairToPairId(image_pair.first, image_pair.second);
            if (image_pair_ids.count(pair_id) > 0) {
              continue;
            }

            image_pair_ids.insert(pair_id);

            const bool exists_matches =
              cache_.ExistsMatches(image_pair.first, image_pair.second);
            const bool exists_inlier_matches =
              cache_.ExistsInlierMatches(image_pair.first, image_pair.second);

            if (exists_matches && exists_inlier_matches) {
              continue;
            }

            num_outputs += 1;

            // If only one of the matches or inlier matches exist, we recompute them
            // from scratch and delete the existing results. This must be done before
            // pushing the jobs to the queue, otherwise database constraints might fail
            // when writing an existing result into the database.

            if (exists_inlier_matches) {
              cache_.DeleteInlierMatches(image_pair.first, image_pair.second);
            }

            colmap::internal::FeatureMatcherData data;
            data.image_id1 = image_pair.first;
            data.image_id2 = image_pair.second;

            if (exists_matches) {
              data.matches = cache_.GetMatches(image_pair.first, image_pair.second);
              cache_.DeleteMatches(image_pair.first, image_pair.second);
              CHECK(verifier_queue_.Push(data));
            } else {
              CHECK(matcher_queue_.Push(data));
            }
          }

          //////////////////////////////////////////////////////////////////////////////
          // Write results to database
          //////////////////////////////////////////////////////////////////////////////

          for (size_t i = 0; i < num_outputs; ++i) {
            const auto output_job = output_queue_.Pop();
            CHECK(output_job.IsValid());
            auto output = output_job.Data();

            if (output.matches.size() < static_cast<size_t>(match_options_.min_num_inliers)) {
              output.matches = {};
            }

            if (output.two_view_geometry.inlier_matches.size() <
                static_cast<size_t>(match_options_.min_num_inliers)) {
              output.two_view_geometry = colmap::TwoViewGeometry();
            }

            cache_.WriteMatches(output.image_id1, output.image_id2, output.matches);
            cache_.WriteTwoViewGeometry(output.image_id1, output.image_id2,
                                         output.two_view_geometry);
          }

          CHECK_EQ(output_queue_.Size(), 0);
          /***********************************************************************************/

          PrintElapsedTime(timer);
        }
      }

      //GetTimer().PrintMinutes();
      matcher_queue_.Wait();
      verifier_queue_.Wait();
      output_queue_.Wait();

      for (auto& matcher : matchers_) {
        matcher->Stop();
      }

      for (auto& verifier : verifiers_) {
        verifier->Stop();
      }

      matcher_queue_.Stop();
      verifier_queue_.Stop();
      output_queue_.Stop();

      for (auto& matcher : matchers_) {
        matcher->Wait();
      }

      for (auto& verifier : verifiers_) {
        verifier->Wait();
      }

    }
    
    colmap::Database database(database_path.toStdString());
    //if (database.NumMatches() == 0) {
    //  msgError("Matching error");
    //  return 1;
    //}
    
    std::vector<colmap::Image> db_images = database.ReadAllImages();
    colmap::image_t image_id_l = 0;
    colmap::image_t image_id_r = 0;
    for (size_t i = 0; i < db_images.size(); i++){
      image_id_l = db_images[i].ImageId();
      for (size_t j = 0; j < i; j++){
        image_id_r = db_images[j].ImageId();

        colmap::FeatureMatches matches = database.ReadMatches(image_id_l, image_id_r);
        if (matches.size() > 0){
          QString path_left = QFileInfo(db_images[i].Name().c_str()).baseName();
          QString path_right = QFileInfo(db_images[j].Name().c_str()).baseName();
          project.addMatchesPair(path_left, path_right);
        }
      }
    }

    database.Close();

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
