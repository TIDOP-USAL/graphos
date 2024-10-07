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
#include "graphos/core/features/featextract.h"
#include "graphos/core/project.h"


#include <tidop/core/msg/message.h>

#include <QDir>
#include <QFile>

#include <atomic>
#include <tidop/core/log.h>

using namespace tl;

namespace graphos
{

FeatureExtractorCommand::FeatureExtractorCommand()
  : Command("featextract", "Feature extraction (SIFT)"),
    mDisableCuda(false)
{
    Sift sift_properties;
    this->addArgument<tl::Path>("prj", 'p', "Project file");
    this->addArgument<int>("max_image_size", 's', "Maximum image size (default = 3200)", 3200);
    this->addArgument<int>("max_features_number", std::string("Maximum number of features to detect (default = ").append(std::to_string(sift_properties.featuresNumber())).append(")"), sift_properties.featuresNumber());
    this->addArgument<int>("octave_resolution", std::string("SIFT: Number of layers in each octave (default = ").append(std::to_string(sift_properties.octaveLayers())).append(")"), sift_properties.octaveLayers());
    this->addArgument<double>("contrast_threshold", std::string("SIFT: Contrast Threshold (default = ").append(std::to_string(sift_properties.contrastThreshold())).append(")"), sift_properties.contrastThreshold());
    this->addArgument<double>("edge_threshold", std::string("SIFT: Threshold used to filter out edge-like features (default = ").append(std::to_string(sift_properties.edgeThreshold())).append(")"), sift_properties.edgeThreshold());  
    //this->addArgument<bool>("domain_size_pooling", std::string("SIFT: domain size pooling (default = true)"));  


#ifdef HAVE_CUDA
    tl::Message::pauseMessages();
    bool cuda_enabled = cudaEnabled(10.0, 3.0);
    tl::Message::resumeMessages();
    if (cuda_enabled)
        this->addArgument<bool>("disable_cuda", "If true disable CUDA (default = false)", mDisableCuda);
    else mDisableCuda = true;
#else
    mDisableCuda = true;
#endif //HAVE_CUDA

    this->addExample("featextract -p 253/253.xml");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

FeatureExtractorCommand::~FeatureExtractorCommand() = default;

bool FeatureExtractorCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<tl::Path>("prj");
        int max_image_size = this->value<int>("max_image_size");
        int max_features_number = this->value<int>("max_features_number");
        int octave_resolution = this->value<int>("octave_resolution");
        double contrast_threshold = this->value<double>("contrast_threshold");
        double edge_threshold = this->value<double>("edge_threshold");
        //bool domain_size_pooling = this->value<bool>("domain_size_pooling");

        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);


        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);
        tl::Path database_path = project.database();

        tl::Path::removeFile(database_path);
        project.removeFeatures();

        std::shared_ptr<FeatureExtractor> feature_extractor;

        if (mDisableCuda) {
            feature_extractor = std::make_shared<SiftCPUDetectorDescriptor>(max_features_number,
                                                                            octave_resolution,
                                                                            edge_threshold,
                                                                            contrast_threshold);

        } else {
            feature_extractor = std::make_shared<SiftCudaDetectorDescriptor>(max_features_number,
                                                                             octave_resolution,
                                                                             edge_threshold,
                                                                             contrast_threshold/*,
                                                                             domain_size_pooling*/);
        }

        FeatureExtractorTask feature_extractor_task(project.images(),
                                                    project.cameras(),
                                                    database_path,
                                                    max_image_size,
                                                    !mDisableCuda,
                                                    feature_extractor);

        connect(&feature_extractor_task, &FeatureExtractorTask::features_extracted,
                [&](size_t imageId, const QString &featuresFile) {
                    project.addFeatures(imageId, featuresFile);
                });

        ProgressBarColor progress(0, project.images().size());
        feature_extractor_task.run(&progress);

        project.setFeatureExtractor(std::dynamic_pointer_cast<Feature>(feature_extractor));
        project.setFeatureExtractorReport(feature_extractor_task.report());
        project.save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos