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


#include "ImageLoaderCommand.h"

#include "graphos/components/images/impl/ImageLoaderTask.h"
#include "graphos/core/task/Progress.h"

#include <tidop/core/app/Message.h>
#include <tidop/core/task/Progress.h>
#include <tidop/core/console/ValuesValidator.h>
#include <tidop/rastertools/io/Reader.h>
#include <tidop/rastertools/io/Metadata.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>
#include <tidop/core/app/Logger.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>

#include <fstream>



namespace graphos
{



ImageLoaderCommand::ImageLoaderCommand()
  : Command("image_manager", "Image manager")
{
    this->addArgument<tl::Path>("prj", 'p', "Path to the project file");
    this->addArgument<tl::Path>("image", 'i', "Image to add to or remove from the project (use with [--delete | -d])", tl::Path(""));
    this->addArgument<tl::Path>("image_list", 'l', "File containing a list of images to add to or remove from the project (use with [--delete | -d])", tl::Path(""));
    auto arg_camera = tl::Argument::make<std::string>("camera", 'c', "Camera type", "OpenCV 1");
    std::vector<std::string> camera_types{"Pinhole 1",
                                          "Pinhole 2",
                                          "Radial 1",
                                          "Radial 2",
                                          "OpenCV 1",
                                          "OpenCV Fisheye",
                                          "OpenCV 2",
                                          "Radial Fisheye 1",
                                          "Radial Fisheye 2",
                                          "Radial 3"};
    arg_camera->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(camera_types));
    this->addArgument(arg_camera);
    this->addOption("delete", 'd', "Remove the specified image(s) from the project", false);
    auto arg_progress_bar = tl::Argument::make<std::string>("progress_bar", "Type of progress bar", "COLOR");
    auto progress_bar_validator = tl::ValuesValidator<std::string>::create({"NORMAL", "COLOR", "PERCENT", "SPINNER", "DISABLE"});
    arg_progress_bar->setValidator(progress_bar_validator);
    this->addArgument(arg_progress_bar);

    this->addExample("image_manager -p 253/253.xml -i image001.jpg");
    this->addExample("image_manager -p 253/253.xml -i image001.jpg -d");
}

ImageLoaderCommand::~ImageLoaderCommand() = default;

bool ImageLoaderCommand::run()
{
    bool r = false;

    auto &log = tl::Logger::instance();

    try {

        auto project_path = this->value<tl::Path>("prj");
        auto image_path = this->value<tl::Path>("image");
        auto image_list_path = this->value<tl::Path>("image_list");
        bool delete_image = this->value<bool>("delete");
        auto progress_bar = this->value<std::string>("progress_bar");
        auto camera_type = this->value<std::string>("camera");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        Project project;
        project.load(project_path);

        std::vector<Image> images;

        if (!image_path.empty()) {
            Image img(image_path);
            if (!project.existImage(Image::id(img)))
                images.push_back(img);
        }

        if (!image_list_path.empty() && image_list_path.exists()) {

            std::ifstream ifs;
            ifs.open(image_list_path.toString(), std::ifstream::in);
            TL_ASSERT(ifs.is_open(), "Images could not be loaded");

            std::string line;
            while (std::getline(ifs, line)) {

                if (line.empty()) continue;
                tl::trim(line);

                Image img(line);
                if (!project.existImage(Image::id(img)))
                    images.push_back(img);
            }

            ifs.close();
            

        }

        if (delete_image) {
            ///// TODO: Borrar las im�genes
            ////std::vector<std::string> _images(image_list.size());
            //for(size_t i = 0; i < image_list.size(); i++) {
            //  //_images[i] = project.findImageByName(image_list[i]).path().toStdString();
            //  project.removeImage(project.imageId(image_list[i]));
            //}
            //QString reconstruction_path = mProject->reconstructionPath();
            //if(!reconstruction_path.isEmpty())
            //  colmapRemoveOrientations(_images, reconstruction_path.toStdString());

            //for(const auto &imageName : imageNames) {
            //  mFeaturesModel->removeFeatures(imageName);
            //  mMatchesModel->removeMatchesPair(imageName);
            //  mView->deleteImage(imageName);
            //  msgInfo("Delete image %s", imageName.toStdString().c_str());
            //}
        } else {

            std::vector<Camera> cameras;

            for (const auto &camera : project.cameras()) {
                cameras.push_back(camera.second);
            }

            LoadImagesTask image_loader_process(&images, &cameras, camera_type/*, project.crs()*/);

            connect(&image_loader_process, &LoadImagesTask::imageAdded,
                    [&](int imageId, int cameraId) {

                        Image image = images[imageId];
                        Camera camera = cameras[cameraId];
                        int id_camera = 0;
                        for (const auto &_camera : project.cameras()) {
                            std::string camera_make = _camera.second.make();
                            std::string camera_model = _camera.second.model();
                            if (camera.make() == camera_make &&
                                camera.model() == camera_model) {
                                id_camera = _camera.first;
                                break;
                            }
                        }
                        if (id_camera == 0)
                            id_camera = project.addCamera(camera);

                        image.setCameraId(id_camera);
                        project.addImage(image);

                    });

            auto progress = getProgressBar(progress_bar, images.size());
            image_loader_process.run(progress.get());

        }

        project.save(project_path);

    } catch (const std::exception &e) {

        tl::printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos