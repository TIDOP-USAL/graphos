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


#include "OrthophotoCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/components/orthophoto/impl/OrthophotoTask.h"
#include "graphos/core/sfm/posesio.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/log.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>


using namespace tl;

namespace graphos
{

OrthophotoCommand::OrthophotoCommand()
  : Command("ortho", "Create orthophoto"),
    mProject(nullptr),
    mDisableCuda(false)
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<double>("gsd", 'g', "Ground sample distance", 0.1);
    //this->addArgument<Path>("dsm", "Digital Surface Model");
    this->addArgument<std::string>("crs", "Coordinate Reference System", "");
    auto arg_interpolation = Argument::make<std::string>("interpolation", 'i', "Interpolation", "BILINEAR");
    std::vector<std::string> interpolation_methods{"NEAREST", "BILINEAR", "BICUBIC"};
    arg_interpolation->setValidator(std::make_shared<ValuesValidator<std::string>>(interpolation_methods));
    this->addArgument(arg_interpolation);

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

    this->addExample("ortho -p 253/253.xml --gsd 0.1");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

OrthophotoCommand::~OrthophotoCommand()
{
    if (mProject) {
        delete mProject;
        mProject = nullptr;
    }
}

bool OrthophotoCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<Path>("prj");
        auto gsd =  this->value<double>("gsd");
        //auto dsm =  this->value<Path>("dsm");
        auto crs =  this->value<std::string>("crs");
        auto interpolation =  this->value<std::string>("interpolation");
        if (!mDisableCuda)
            mDisableCuda = this->value<bool>("disable_cuda");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        mProject = new ProjectImp;
        mProject->load(project_path);

        auto dsm = mProject->dem().dsmPath;

        tl::Path orthophoto_path(mProject->projectFolder());
        orthophoto_path.append("ortho");
		
        tl::Path ground_points_path(mProject->reconstructionPath());
        ground_points_path.append("ground_points.bin");

        tl::Point3<double> offset = offsetRead(mProject->offset());

        if (crs.empty()){

            // Esto no tiene que hacerse ya que vamos a tener las coordenadas geograficas directamente
            auto epsg_geographic = std::make_shared<tl::Crs>("EPSG:4326");
            auto epsg_geocentric = std::make_shared<tl::Crs>("EPSG:4978");
            tl::CrsTransform crs_transfom_geocentric_to_geographic(epsg_geocentric, epsg_geographic);
            auto lla = crs_transfom_geocentric_to_geographic.transform(offset);

            //auto zone = tl::utmZoneFromLonLat(lla.x, lla.y);
            int zone = tl::utmZoneFromLongitude(lla.x);
            crs = "EPSG:326";
            crs.append(std::to_string(zone));
        }

        OrthophotoTask orthophoto_task(gsd,
                                       images(),
                                       mProject->cameras(),
                                       orthophoto_path,
                                       dsm,
                                       offset,
                                       crs,
                                       interpolation,
                                       !mDisableCuda);
        orthophoto_task.run();

        tl::Path orthophoto_file = orthophoto_path;
        orthophoto_file.append("dsm.tif");
        if (orthophoto_file.exists()) {
            //mProject->dtm().dsmPath = orthophoto_file;
        }

        //mProject->dtm().gsd = gsd;
        mProject->save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

auto OrthophotoCommand::images() -> std::vector<Image>
{
    std::vector<Image> images;

    //tl::Point3<double> offset;

    //std::ifstream ifs;
    //ifs.open(mProject->offset().toString(), std::ifstream::in);
    //if(ifs.is_open()) {

    //    ifs >> offset.x >> offset.y >> offset.z;

    //    ifs.close();
    //}

    for(const auto &image : mProject->images()) {

        Image photo(image.second);
        size_t image_id = image.first;

        if(mProject->isPhotoOriented(image_id)) {
            CameraPose photoOrientation = mProject->photoOrientation(image_id);
            auto rotation_matrix = photoOrientation.rotationMatrix();
            rotation_matrix.at(1, 0) = -photoOrientation.rotationMatrix().at(1, 0);
            rotation_matrix.at(1, 1) = -photoOrientation.rotationMatrix().at(1, 1);
            rotation_matrix.at(1, 2) = -photoOrientation.rotationMatrix().at(1, 2);
            rotation_matrix.at(2, 0) = -photoOrientation.rotationMatrix().at(2, 0);
            rotation_matrix.at(2, 1) = -photoOrientation.rotationMatrix().at(2, 1);
            rotation_matrix.at(2, 2) = -photoOrientation.rotationMatrix().at(2, 2);
            photoOrientation.setRotationMatrix(rotation_matrix);

            photoOrientation.setPosition(photoOrientation.position() /*+ offset*/);

            photo.setCameraPose(photoOrientation);

            images.push_back(photo);
        }

    }

    return images;
}

} // namespace graphos