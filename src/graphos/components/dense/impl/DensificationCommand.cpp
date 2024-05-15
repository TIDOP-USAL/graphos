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


#include "DensificationCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/core/dense/CmvsPmvs.h"
#include "graphos/core/dense/Smvs.h"
#include "graphos/core/dense/mvs.h"

#include <tidop/core/msg/message.h>

#include <QFileInfo>
#include <tidop/core/log.h>

using namespace tl;

namespace graphos
{


DensificationCommand::DensificationCommand()
  : Command("dense", "Create a dense point cloud"),
    mDisableCuda(false)
{
    this->addArgument<tl::Path>("prj", 'p', "Project file");
    auto arg_method = tl::Argument::make<std::string>("method", 'm', "Densification Method", "mvs");
    std::vector<std::string> methods{"mvs", "pmvs", "smvs"};
    arg_method->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(methods));
    this->addArgument(arg_method);
    this->addArgument<int>("mvs:resolution_level", "Resolution Level", 1);
    this->addArgument<int>("mvs:min_resolution", "Min Resolution", 640);
    this->addArgument<int>("mvs:max_resolution", "Max Resolution", 3200);
    this->addArgument<int>("mvs:number_views", "Number Views", 5);
    this->addArgument<int>("mvs:number_views_fuse", "Number Views Fuse", 3);
    this->addArgument<bool>("mvs:estimate-colors", "Estimate color", true);
    this->addArgument<bool>("mvs:estimate-normals", "Estimate normals", true);
    this->addArgument<bool>("pmvs:visibility", "Use Visibility Information", true);
    this->addArgument<int>("pmvs:images_per_cluster", "Images per cluster", 100);
    this->addArgument<int>("pmvs:level", "Level", 1);
    this->addArgument<int>("pmvs:cell_size", "Cell Size", 2);
    this->addArgument<double>("pmvs:threshold", "Threshold", 0.7);
    this->addArgument<int>("pmvs:window_size", "Window Size", 7);
    this->addArgument<int>("pmvs:minimun_images", "Minimun Image Number", 3);
    this->addArgument<bool>("pmvs:original_depth", "Image Original Depth", false);
    this->addArgument<int>("smvs:input_image_scale", "Input Image Scale", 1);
    this->addArgument<int>("smvs:output_depth_scale", "Output Depth Scale", 2);
    this->addArgument<bool>("smvs:shading_optimization", "Shading Based Optimization", false);
    this->addArgument<bool>("smvs:sgm", "Semi-global Matching", true);
    this->addArgument<double>("smvs:smooth_factor", "Surface Smoothing Factor", 1.0);

#ifdef HAVE_CUDA
    tl::Message::pauseMessages();
    bool cuda_enabled = cudaEnabled(10.0, 3.0);
    tl::Message::resumeMessages();
    if (cuda_enabled)
        this->addArgument<bool>("disable_cuda", "If true disable CUDA", mDisableCuda);
    else mDisableCuda = true;
#else
    mDisableCuda = true;
#endif //HAVE_CUDA

    this->addExample("dense -p 253/253.xml --method PMVS");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

DensificationCommand::~DensificationCommand() = default;

bool DensificationCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<tl::Path>("prj");
        auto densification_method = this->value<std::string>("method");
        auto pmvs_use_visibility_information = this->value<bool>("pmvs:visibility");
        auto pmvs_images_per_cluster = this->value<int>("pmvs:images_per_cluster");
        auto pmvs_level = this->value<int>("pmvs:level");
        auto pmvs_cell_size = this->value<int>("pmvs:cell_size");
        auto pmvs_threshold = this->value<double>("pmvs:threshold");
        auto pmvs_window_size = this->value<int>("pmvs:window_size");
        auto pmvs_minimun_image_number = this->value<int>("pmvs:minimun_images");
        auto pmvs_image_original_depth = this->value<bool>("pmvs:original_depth");
        auto smvs_input_image_scale = this->value<int>("smvs:input_image_scale");
        auto smvs_output_depth_scale = this->value<int>("smvs:output_depth_scale");
        auto smvs_shading_based_optimization = this->value<bool>("smvs:shading_optimization");
        auto smvs_semi_global_matching = this->value<bool>("smvs:sgm");
        auto smvs_surface_smoothing_factor = this->value<double>("smvs:smooth_factor");
	    auto mvs_resolution_level = this->value<int>("mvs:resolution_level");
	    auto mvs_min_resolution = this->value<int>("mvs:min_resolution");
	    auto mvs_max_resolution = this->value<int>("mvs:max_resolution");
	    auto mvs_number_views = this->value<int>("mvs:number_views");
	    auto mvs_number_views_fuse = this->value<int>("mvs:number_views_fuse");
        auto mvs_estimate_colors = this->value<int>("mvs:estimate-colors");
        auto mvs_estimate_normals = this->value<int>("mvs:estimate-normals");


        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);


        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);

        tl::Path dense_path(project.projectFolder());
        dense_path.append("dense");

        tl::Path ground_points_path(project.reconstructionPath());
        ground_points_path.append("ground_points.bin");

        auto reader = GroundPointsReaderFactory::create("GRAPHOS");
        reader->read(ground_points_path);

        auto ground_points = reader->points();

        std::shared_ptr<tl::Task> dense_task;

        if (densification_method == "pmvs") {

            tl::Message::info("PMVS Properties:");
            tl::Message::info("- Use Visibility Information: {}", pmvs_use_visibility_information ? "True" : "False");
            tl::Message::info("- Images per cluster: {}", pmvs_images_per_cluster);
            tl::Message::info("- Level: {}", pmvs_level);
            tl::Message::info("- Cell Size: {}", pmvs_cell_size);
            tl::Message::info("- Threshold: {}", pmvs_threshold);
            tl::Message::info("- Window Size: {}", pmvs_window_size);
            tl::Message::info("- Minimun Image Number: {}", pmvs_minimun_image_number);
            tl::Message::info("- Image Original Depth: {}", pmvs_image_original_depth ? "true" : "false");

            dense_path.append("pmvs");

            auto pmvs = std::make_shared<CmvsPmvsDensifier>(project.images(),
                                                            project.cameras(),
                                                            project.poses(),
                                                            ground_points,
                                                            dense_path,
                                                            project.database(),
                                                            !mDisableCuda);

            pmvs->setUseVisibilityInformation(pmvs_use_visibility_information);
            pmvs->setImagesPerCluster(pmvs_images_per_cluster);
            pmvs->setLevel(pmvs_level);
            pmvs->setCellSize(pmvs_cell_size);
            pmvs->setThreshold(pmvs_threshold);
            pmvs->setWindowSize(pmvs_window_size);
            pmvs->setMinimunImageNumber(pmvs_minimun_image_number);

            project.setDensification(pmvs);

            dense_task = std::move(pmvs);

        } else if (densification_method == "smvs") {

            tl::Message::info("SMVS Properties:");
            tl::Message::info("- Input Image Scale: {}", smvs_input_image_scale);
            tl::Message::info("- Output Depth Scale: {}", smvs_output_depth_scale);
            tl::Message::info("- Shading Based Optimization: {}", smvs_shading_based_optimization ? "true" : "false");
            tl::Message::info("- Semi-global Matching: {}", smvs_semi_global_matching ? "true" : "false");
            tl::Message::info("- Surface Smoothing Factor: {}", smvs_surface_smoothing_factor);

            dense_path.append("smvs");

            auto smvs = std::make_shared<SmvsDensifier>(project.images(),
                                                        project.cameras(),
                                                        project.poses(),
                                                        ground_points,
                                                        dense_path,
                                                        !mDisableCuda);

            smvs->setInputImageScale(smvs_input_image_scale);
            smvs->setOutputDepthScale(smvs_output_depth_scale);
            smvs->setShadingBasedOptimization(smvs_shading_based_optimization);
            smvs->setSemiGlobalMatching(smvs_semi_global_matching);
            smvs->setSurfaceSmoothingFactor(smvs_surface_smoothing_factor);

            project.setDensification(smvs);

            dense_task = smvs;

        } else if (densification_method == "mvs") {

            tl::Message::info("MVS Properties:");
            tl::Message::info("- Resolution Level: {}", mvs_resolution_level);
            tl::Message::info("- Min Resolution: {}", mvs_min_resolution);
            tl::Message::info("- Max Resolution: {}", mvs_max_resolution);
            tl::Message::info("- Number Views: {}", mvs_number_views);
            tl::Message::info("- Number Views Fuse: {}", mvs_number_views_fuse);
            tl::Message::info("- Estimate normals: {}", mvs_estimate_colors);
            tl::Message::info("- Estimate colors: {}", mvs_estimate_normals);
            
            dense_path.append("mvs");

            auto mvs = std::make_shared<MvsDensifier>(project.images(),
                                                      project.cameras(),
                                                      project.poses(),
                                                      ground_points,
                                                      dense_path,
                                                      project.database(),
                                                      !mDisableCuda);

            mvs->setMaxResolution(mvs_max_resolution);
            mvs->setMinResolution(mvs_min_resolution);
            mvs->setNumberViews(mvs_number_views);
            mvs->setNumberViewsFuse(mvs_number_views_fuse);
            mvs->setResolutionLevel(mvs_resolution_level);
            mvs->setEstimateColors(mvs_estimate_colors);
            mvs->setEstimateNormals(mvs_estimate_normals);

            project.setDensification(mvs);

            dense_task = mvs;

        } else {
            throw std::runtime_error("Densification Method not valid");
        }

        dense_task->run();
        
        dense_path = dynamic_cast<DensifierBase const *>(dense_task.get())->denseModel();
        project.setDenseModel(dense_path);
        project.setDenseReport(dynamic_cast<DensifierBase const *>(dense_task.get())->report());
        project.save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos