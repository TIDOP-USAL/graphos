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

#include "FeatureMatchingCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/features/matching.h"
#include "graphos/core/project.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/chrono.h>

#include <colmap/feature/matching.h>
#include <colmap/base/database.h>

#include <QFileInfo>

using namespace tl;

namespace graphos
{

FeatureMatchingCommand::FeatureMatchingCommand()
  : Command("featmatch", "Feature Matching"),
    mDisableCuda(false)
{
    FeatureMatchingProperties featureMatchingProperties;
    this->addArgument<std::string>("prj", 'p', "Project file");
    this->addArgument<double>("ratio", 'r', "Ratio", featureMatchingProperties.ratio());
    this->addArgument<double>("distance", 'd', "Distance", featureMatchingProperties.distance());
    this->addArgument<double>("max_error", 'e', "Maximun error", featureMatchingProperties.maxError());
    this->addArgument<double>("confidence", 'c', "Confidence", featureMatchingProperties.confidence());
    this->addArgument<bool>("cross_check", 'x', "If true, cross checking is enabled", featureMatchingProperties.crossCheck());
    this->addArgument<bool>("exhaustive_matching", "Force exhaustive matching (default = false)", false);

#ifdef HAVE_CUDA
    tl::Message::instance().pauseMessages();
    bool cuda_enabled = cudaEnabled(10.0, 3.0);
    tl::Message::instance().resumeMessages();
    if (cuda_enabled)
        this->addArgument<bool>("disable_cuda", "If true disable CUDA", mDisableCuda);
    else mDisableCuda = true;
#else
    mDisableCuda = true;
#endif //HAVE_CUDA

    this->addExample("featmatch --p 253/253.xml");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

FeatureMatchingCommand::~FeatureMatchingCommand()
{

}

bool FeatureMatchingCommand::run()
{
    bool r = false;

    QString file_path;
    QString project_path;

    try {

        tl::Path project_path = this->value<std::string>("prj");
        double ratio = this->value<double>("ratio");
        double distance = this->value<double>("distance");
        double max_error = this->value<double>("max_error");
        double confidence = this->value<double>("confidence");
        double cross_check = this->value<bool>("cross_check");
        double exhaustive_matching = this->value<bool>("exhaustive_matching");
        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);
        tl::Path database_file = project.database();

        {
            colmap::Database database(database_file.toString());
            database.ClearMatches();
            database.ClearTwoViewGeometries();
            database.Close();
            project.removeMatchesPair();
        }


        std::shared_ptr<FeatureMatching> feature_matching_properties = std::make_shared<FeatureMatchingProperties>();
        feature_matching_properties->setRatio(ratio);
        feature_matching_properties->setDistance(distance);
        feature_matching_properties->setMaxError(max_error);
        feature_matching_properties->setConfidence(confidence);
        feature_matching_properties->enableCrossCheck(cross_check);

        bool spatial_matching = false;

        if (!exhaustive_matching) {
            auto it = project.images().begin();
            if (it != project.images().end()) {
                CameraPose cameraPosition = it->second.cameraPose();
                if (!cameraPosition.isEmpty())
                    spatial_matching = true;
            }
        }

        if (spatial_matching) {
            SpatialMatchingTask featmatching_process(project.database(),
                                                     !mDisableCuda,
                                                     feature_matching_properties);
            featmatching_process.run();
        } else {
            FeatureMatchingTask featmatching_process(project.database(),
                                                     !mDisableCuda,
                                                     feature_matching_properties);
            featmatching_process.run();
        }

        project.setFeatureMatching(feature_matching_properties);
        writeMatchPairs(&project);
        project.save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    return r;
}

void FeatureMatchingCommand::writeMatchPairs(Project *project)
{
    tl::Path database_file = project->database();
    colmap::Database database(database_file.toString());
    std::vector<colmap::Image> db_images = database.ReadAllImages();
    colmap::image_t colmap_image_id_l = 0;
    colmap::image_t colmap_image_id_r = 0;

    for (size_t i = 0; i < db_images.size(); i++) {

        colmap_image_id_l = db_images[i].ImageId();

        for (size_t j = 0; j < i; j++) {
            colmap_image_id_r = db_images[j].ImageId();

            colmap::FeatureMatches matches = database.ReadMatches(colmap_image_id_l, colmap_image_id_r);

            if (matches.size() > 0) {

                tl::Path colmap_image1(db_images[i].Name());
                tl::Path colmap_image2(db_images[j].Name());

                bool find_id_1 = false;
                bool find_id_2 = false;
                size_t image_id_1 = 0;
                size_t image_id_2 = 0;

                for (const auto &image_pair : project->images()) {

                    tl::Path image(image_pair.second.path().toStdString());

                    if (!find_id_1 && image.equivalent(colmap_image1)) {
                        find_id_1 = true;
                        image_id_1 = image_pair.first;
                    }

                    if (!find_id_2 && image.equivalent(colmap_image2)) {
                        find_id_2 = true;
                        image_id_2 = image_pair.first;
                    }

                    if (find_id_1 && find_id_2) {
                        project->addMatchesPair(image_id_1, image_id_2);
                    }
                }

            }

        }

    }

    database.Close();
}

} // namespace graphos