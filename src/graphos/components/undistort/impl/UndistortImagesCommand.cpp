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


#include "UndistortImagesCommand.h"

#include <tidop/core/log.h>
#include <tidop/core/progress.h>

#include "graphos/core/camera/Undistort.h"
#include "graphos/core/utils.h"

namespace graphos
{

UndistortImagesCommand::UndistortImagesCommand()
    : Command("undistort", "Undistort images"),
    mDisableCuda(false)
{
    this->addArgument<std::string>("prj", 'p', "Project file");
    this->addArgument<std::string>("output_path", 'o', "Output directory for undistorted images.");

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

    this->addExample("undistort --p 253/253.xml --output_path undistort");
}

UndistortImagesCommand::~UndistortImagesCommand()
{
}

bool UndistortImagesCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<std::string>("prj");
        tl::Path output_path = this->value<std::string>("output_path");
        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path.toString());

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(project_path);

        UndistortImages task(project.images(),
                             project.cameras(),
                             QString::fromStdWString(output_path.toWString()),
                             UndistortImages::Format::tiff,
                             mDisableCuda);

        tl::ProgressBarColor progress(0, project.images().size());
        task.run(&progress);

    } catch (const std::exception &e) {

        tl::printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos