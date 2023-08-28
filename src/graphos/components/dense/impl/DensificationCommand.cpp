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

#include <tidop/core/chrono.h>
#include <tidop/core/msg/message.h>

#include <QFileInfo>

using namespace tl;

namespace graphos
{


DensificationCommand::DensificationCommand()
  : Command("dense", "Create a dense point cloud"),
    mDisableCuda(false),
    mProject(nullptr)
{
    this->addArgument<std::string>("prj", 'p', "Project file");
    auto arg_method = tl::Argument::make<std::string>("method", 'm', "Densification Method", "mvs");
    std::vector<std::string> methods{"mvs", "pmvs", "smvs"};
    arg_method->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(methods));
    this->addArgument(arg_method);
    this->addArgument<int>("mvs:resolution_level", "Resolution Level", 1);
    this->addArgument<int>("mvs:min_resolution", "Min Resolution", 640);
    this->addArgument<int>("mvs:max_resolution", "Max Resolution", 3200);
    this->addArgument<int>("mvs:number_views", "Number Views", 5);
    this->addArgument<int>("mvs:number_views_fuse", "Number Views Fuse", 3);
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
    tl::Message::instance().pauseMessages();
    bool cuda_enabled = cudaEnabled(10.0, 3.0);
    tl::Message::instance().resumeMessages();
    if (cuda_enabled)
        this->addArgument<bool>("disable_cuda", "If true disable CUDA", mDisableCuda);
    else mDisableCuda = true;
#else
    mDisableCuda = true;
#endif //HAVE_CUDA

    this->addExample("dense -p 253/253.xml --method PMVS");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

DensificationCommand::~DensificationCommand()
{
    if (mProject) {
        delete mProject;
        mProject = nullptr;
    }
}

bool DensificationCommand::run()
{
    bool r = false;

    QString file_path;
    QString project_path;

    try {

        //tl::Chrono chrono("Densification finished");
        //chrono.run();

        tl::Path projectFile = this->value<std::string>("prj");
        std::string densificationMethod = this->value<std::string>("method");
        bool pmvsUseVisibilityInformation = this->value<bool>("pmvs:visibility");
	    int pmvsImagesPerCluster = this->value<int>("pmvs:images_per_cluster");
	    int pmvsLevel = this->value<int>("pmvs:level");
	    int pmvsCellSize = this->value<int>("pmvs:cell_size");
	    double pmvsThreshold = this->value<double>("pmvs:threshold");
	    int pmvsWindowSize = this->value<int>("pmvs:window_size");
	    int pmvsMinimunImageNumber = this->value<int>("pmvs:minimun_images");
	    bool pmvsImageOriginalDepth = this->value<bool>("pmvs:original_depth");
	    int smvsInputImageScale = this->value<int>("smvs:input_image_scale");
	    int smvsOutputDepthScale = this->value<int>("smvs:output_depth_scale");
	    bool smvsShadingBasedOptimization = this->value<bool>("smvs:shading_optimization");
	    bool smvsSemiGlobalMatching = this->value<bool>("smvs:sgm");
	    double smvsSurfaceSmoothingFactor = this->value<double>("smvs:smooth_factor");
	    int mvsResolutionLevel = this->value<int>("mvs:resolution_level");
	    int mvsMinResolution = this->value<int>("mvs:min_resolution");
	    int mvsMaxResolution = this->value<int>("mvs:max_resolution");
	    int mvsNumberViews = this->value<int>("mvs:number_views");
	    int mvsNumberViewsFuse = this->value<int>("mvs:number_views_fuse");
        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        TL_ASSERT(projectFile.exists(), "Project doesn't exist");
        TL_ASSERT(projectFile.isFile(), "Project file doesn't exist");

        mProject = new ProjectImp;
        mProject->load(projectFile);

        tl::Path dense_path(mProject->projectFolder());
        dense_path.append("dense");

        tl::Path ground_points_path(mProject->reconstructionPath());
        ground_points_path.append("ground_points.bin");

        std::unique_ptr<GroundPointsReader> reader = GroundPointsReaderFactory::create("GRAPHOS");
        reader->read(ground_points_path);

        std::vector<GroundPoint> ground_points = reader->points();

        std::shared_ptr<tl::Task>  dense_task;

        if (densificationMethod == "pmvs") {

            tl::Message::info("PMVS Properties:");
            tl::Message::info("- Use Visibility Information: {}", pmvsUseVisibilityInformation ? "True" : "False");
            tl::Message::info("- Images per cluster: {}", pmvsImagesPerCluster);
            tl::Message::info("- Level: {}", pmvsLevel);
            tl::Message::info("- Cell Size: {}", pmvsCellSize);
            tl::Message::info("- Threshold: {}", pmvsThreshold);
            tl::Message::info("- Window Size: {}", pmvsWindowSize);
            tl::Message::info("- Minimun Image Number: {}", pmvsMinimunImageNumber);
            tl::Message::info("- Image Original Depth: {}", pmvsImageOriginalDepth ? "true" : "false");

            dense_path.append("pmvs");

            auto pmvs = std::make_shared<CmvsPmvsDensifier>(mProject->images(),
                                                            mProject->cameras(),
                                                            mProject->poses(),
                                                            ground_points,
                                                            dense_path,
                                                            mProject->database(),
                                                            !mDisableCuda);

            pmvs->setUseVisibilityInformation(pmvsUseVisibilityInformation);
            pmvs->setImagesPerCluster(pmvsImagesPerCluster);
            pmvs->setLevel(pmvsLevel);
            pmvs->setCellSize(pmvsCellSize);
            pmvs->setThreshold(pmvsThreshold);
            pmvs->setWindowSize(pmvsWindowSize);
            pmvs->setMinimunImageNumber(pmvsMinimunImageNumber);

            mProject->setDensification(pmvs);
            //mProject->setDenseModel(pmvs->denseModel());

            dense_task = std::move(pmvs);

        } else if (densificationMethod == "smvs") {

            tl::Message::info("SMVS Properties:");
            tl::Message::info("- Input Image Scale: {}", smvsInputImageScale);
            tl::Message::info("- Output Depth Scale: {}", smvsOutputDepthScale);
            tl::Message::info("- Shading Based Optimization: {}", smvsShadingBasedOptimization ? "true" : "false");
            tl::Message::info("- Semi-global Matching: {}", smvsSemiGlobalMatching ? "true" : "false");
            tl::Message::info("- Surface Smoothing Factor: {}", smvsSurfaceSmoothingFactor);

            dense_path.append("smvs");

            auto smvs = std::make_shared<SmvsDensifier>(mProject->images(),
                                                        mProject->cameras(),
                                                        mProject->poses(),
                                                        ground_points,
                                                        dense_path,
                                                        !mDisableCuda);

            smvs->setInputImageScale(smvsInputImageScale);
            smvs->setOutputDepthScale(smvsOutputDepthScale);
            smvs->setShadingBasedOptimization(smvsShadingBasedOptimization);
            smvs->setSemiGlobalMatching(smvsSemiGlobalMatching);
            smvs->setSurfaceSmoothingFactor(smvsSurfaceSmoothingFactor);

            mProject->setDensification(smvs);
            //mProject->setDenseModel(smvs->denseModel());

            dense_task = smvs;

        } else if (densificationMethod == "mvs") {

            tl::Message::info("MVS Properties:");
            tl::Message::info("- Resolution Level: {}", mvsResolutionLevel);
            tl::Message::info("- Min Resolution: {}", mvsMinResolution);
            tl::Message::info("- Max Resolution: {}", mvsMaxResolution);
            tl::Message::info("- Number Views: {}", mvsNumberViews);
            tl::Message::info("- Number Views Fuse: {}", mvsNumberViewsFuse);

            dense_path.append("mvs");

            auto mvs = std::make_shared<MvsDensifier>(mProject->images(),
                                                      mProject->cameras(),
                                                      mProject->poses(),
                                                      ground_points,
                                                      dense_path,
                                                      mProject->database(),
                                                      !mDisableCuda);

            mvs->setMaxResolution(mvsMaxResolution);
            mvs->setMinResolution(mvsMinResolution);
            mvs->setNumberViews(mvsNumberViews);
            mvs->setNumberViewsFuse(mvsNumberViews);
            mvs->setResolutionLevel(mvsResolutionLevel);

            mProject->setDensification(mvs);
            //mProject->setDenseModel(mvs->denseModel());

            dense_task = mvs;

        } else {
            throw std::runtime_error("Densification Method not valid");
        }

        dense_task->run();
        
        dense_path = dynamic_cast<DensifierBase const *>(dense_task.get())->denseModel();
        mProject->setDenseModel(dense_path);

        mProject->save(projectFile);

        //chrono.stop();

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    return r;
}

} // namespace graphos