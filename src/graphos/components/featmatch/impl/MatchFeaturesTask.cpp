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

#include "graphos/components/featmatch/impl/MatchFeaturesTask.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/base/Exception.h>
#include <tidop/core/task/Progress.h>

TL_DISABLE_WARNINGS
#include <colmap/feature/sift.h>
#include <colmap/feature/utils.h>
#include <colmap/retrieval/visual_index.h>
#include <colmap/util/cuda.h>
#include <colmap/util/misc.h>
#include <colmap/estimators/two_view_geometry.h>
#include <colmap/controllers/feature_matching_utils.h>

TL_DEFAULT_WARNINGS

namespace graphos
{


MatchFeaturesTask::MatchFeaturesTask(tl::Path database,
                                         bool cuda,
                                         const std::shared_ptr<FeatureMatching> &featureMatching)
  : mDatabase(std::move(database)),
    bUseCuda(cuda),
    mFeatureMatching(featureMatching)
{
}

MatchFeaturesTask::~MatchFeaturesTask() = default;

void MatchFeaturesTask::execute(tl::Progress *progressBar, std::stop_token stopToken)
{
    try {

        auto database = colmap::Database::Open(mDatabase.toUtf8());
        TL_ASSERT(database->NumKeypoints() > 0, "Keypoints not found in the database");

        colmap::FeatureMatchingOptions matching_options;
        matching_options.sift = std::make_shared<colmap::SiftMatchingOptions>();
        matching_options.sift->cross_check = mFeatureMatching->crossCheck();
        matching_options.sift->max_ratio = mFeatureMatching->ratio();
        matching_options.sift->max_distance = mFeatureMatching->distance();
        matching_options.use_gpu = bUseCuda;

        colmap::TwoViewGeometryOptions geometry_options;
        geometry_options.min_num_inliers = 15;
        geometry_options.ransac_options.confidence = mFeatureMatching->confidence();
        geometry_options.ransac_options.max_error = mFeatureMatching->maxError();


        colmap::ExhaustivePairingOptions pairing_options;
        auto cache = std::make_shared<colmap::FeatureMatcherCache>(pairing_options.CacheSize(), database);

        colmap::FeatureMatcherController matcher_(matching_options, geometry_options, cache);
        TL_ASSERT(matcher_.Setup(), "");

        auto pair_generator = std::make_unique<colmap::ExhaustivePairGenerator>(pairing_options, cache);

        while (!pair_generator->HasFinished()) {

            interruptionPoint(stopToken); // Emite una excepción
            pausePoint(stopToken);

            const auto &image_pairs = pair_generator->Next();
            matcher_.Match(image_pairs);

            if (progressBar) (*progressBar)();
        }

        size_t num_matches = database->NumMatches();
        database->Close();

        if (status() != Status::stopping) {

            TL_ASSERT(num_matches > 0, "Matching points not detected");

            mReport.matches = static_cast<int>(num_matches);
            mReport.cuda = bUseCuda;
            mReport.time = this->time();

            tl::Message::success("Feature Matching finished in {:.2} minutes", mReport.time / 60.);
            tl::Message::info(" - Total matches: {}", num_matches);

        }

    } catch (...) {

        TL_THROW_EXCEPTION_WITH_NESTED("Feature Matching error");
    }
}

auto MatchFeaturesTask::report() const -> FeatureMatchingReport
{
    return mReport;
}

} // namespace graphos
