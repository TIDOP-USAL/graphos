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


#include "FeatureExtractorCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/features/featio.h"
#include "graphos/core/project/Project.h"
#include "graphos/core/io/ProjectReader.h"
#include "graphos/core/io/ProjectWriter.h"
#include "graphos/core/task/Progress.h"
#include "graphos/components/featextract/impl/ExtractFeaturesTask.h"
#include "graphos/components/featextract/impl/SiftCPUExtractor.h"
#include "graphos/components/featextract/impl/SiftGPUExtractor.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/console/ValuesValidator.h>
#include <tidop/core/app/Logger.h>

namespace graphos
{

FeatureExtractorCommand::FeatureExtractorCommand()
  : Command("featextract", "Feature extraction (SIFT)"),
    mDisableCuda(false)
{
    Sift sift_properties;
    this->addArgument<tl::Path>("prj", 'p', "Path to the project file");
    this->addArgument<int>("max_image_size", 's', "Maximum image size (default: 3200)", 3200);
    this->addArgument<int>("max_features_number", std::string("Maximum number of features to detect (default = ").append(std::to_string(sift_properties.featuresNumber())).append(")"), sift_properties.featuresNumber());
    this->addArgument<int>("octave_resolution", std::string("SIFT: Number of layers per octave (default = ").append(std::to_string(sift_properties.octaveLayers())).append(")"), sift_properties.octaveLayers());
    this->addArgument<double>("contrast_threshold", std::string("SIFT: Contrast threshold (default = ").append(std::to_string(sift_properties.contrastThreshold())).append(")"), sift_properties.contrastThreshold());
    this->addArgument<double>("edge_threshold", std::string("SIFT: Edge threshold used to filter out edge-like features (default = ").append(std::to_string(sift_properties.edgeThreshold())).append(")"), sift_properties.edgeThreshold());  
    auto arg_progress_bar = tl::Argument::make<std::string>("progress_bar", "Type of progress bar", "COLOR");
    auto progress_bar_validator = tl::ValuesValidator<std::string>::create({"NORMAL", "COLOR", "PERCENT", "SPINNER", "DISABLE"});
    arg_progress_bar->setValidator(progress_bar_validator);
    this->addArgument(arg_progress_bar);

#ifdef HAVE_CUDA
    tl::Message::pauseMessages();
    bool cuda_enabled = cudaEnabled(10.0, 3.0);
    tl::Message::resumeMessages();
    if (cuda_enabled)
        this->addArgument<bool>("disable_cuda", "Disable CUDA acceleration (default = false)", mDisableCuda);
    else mDisableCuda = true;
#else
    mDisableCuda = true;
#endif //HAVE_CUDA

    this->addExample("featextract -p 253/253.xml");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

FeatureExtractorCommand::~FeatureExtractorCommand() = default;

auto FeatureExtractorCommand::run() -> bool
{
    bool has_error = false;

    auto &log = tl::Logger::instance();

    try {

        tl::Path project_path = this->value<tl::Path>("prj");
        int max_image_size = this->value<int>("max_image_size");
        int max_features_number = this->value<int>("max_features_number");
        int octave_resolution = this->value<int>("octave_resolution");
        double contrast_threshold = this->value<double>("contrast_threshold");
        double edge_threshold = this->value<double>("edge_threshold");
        auto progress_bar = this->value<std::string>("progress_bar");

        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);


        TL_ASSERT(project_path.exists(), "Project file doesn't exist: {}", project_path.toString());

        Project project;
        ProjectReader reader;
        reader.read(project_path, project);

        tl::Path database_path = project.info().database();
        tl::Path::removeFile(database_path);
        project.removeFeatures();


        auto sift_config = std::make_shared<Sift>();
        sift_config->setFeaturesNumber(max_features_number);
        sift_config->setOctaveLayers(octave_resolution);
        sift_config->setContrastThreshold(contrast_threshold);
        sift_config->setEdgeThreshold(edge_threshold);


        std::shared_ptr<FeatureExtractor> feature_extractor;
        if (mDisableCuda) {
            feature_extractor = std::make_shared<SiftCPUExtractor>(*sift_config);
        } else {
            feature_extractor = std::make_shared<SiftGPUExtractor>(*sift_config);
        }


        ExtractFeaturesTask feature_extractor_task(project.images(),
                                                   project.cameras(),
                                                   database_path,
                                                   max_image_size,
                                                   !mDisableCuda,
                                                   feature_extractor);

        auto &features = project.features();

        connect(&feature_extractor_task, &ExtractFeaturesTask::features_extracted,
                [&](size_t imageId, const QString &featuresFile) {
                features.add(imageId, featuresFile.toStdString());
                });

        auto progress = getProgressBar(progress_bar, project.images().size());
        feature_extractor_task.run(progress.get());


        project.setFeatureConfig(sift_config);
        project.setFeatureReport(feature_extractor_task.report());

        ProjectWriter writer;
        writer.write(project_path, project);

    } catch (const std::exception &e) {

        tl::printException(e);

        has_error = true;
    }

    log.close();

    return has_error;
}

} // namespace graphos