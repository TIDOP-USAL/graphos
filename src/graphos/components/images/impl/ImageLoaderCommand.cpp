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

#include <tidop/core/msg/message.h>
#include <tidop/core/chrono.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <QVariant>

#include <fstream>

namespace graphos
{

ImageLoaderCommand::ImageLoaderCommand()
  : Command("image_manager", "Image manager")
{
    this->addArgument<std::string>("prj", 'p', "Project file");
    this->addArgument<std::string>("image", 'i', "Image added or removed (with option [--delete|-d]) from project.", "");
    this->addArgument<std::string>("image_list", 'l', "List of images added or removed (with option [--delete|-d]) from project.", "");
    this->addArgument<bool>("delete", 'd', "Delete an image in project", false);
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

    this->addExample("image_manager -p 253/253.xml -i image001.jpg");
    this->addExample("image_manager -p 253/253.xml -i image001.jpg -d");
}

ImageLoaderCommand::~ImageLoaderCommand()
{
}

bool ImageLoaderCommand::run()
{
    bool r = false;

    QString file_path;
    QString project_path;

    try {

        tl::Path prj_path = this->value<std::string>("prj");
        tl::Path image_path = this->value<std::string>("image");
        tl::Path image_list_path = this->value<std::string>("image_list");
        bool delete_image = this->value<bool>("delete");
        std::string camera_type = this->value<std::string>("camera");

        TL_ASSERT(prj_path.exists(), "Project doesn't exist");
        TL_ASSERT(prj_path.isFile(), "Project file doesn't exist");

        ProjectImp project;
        project.load(prj_path);

        std::vector<QString> image_list;
        image_list.reserve(1000);

        if (!image_path.empty()) image_list.push_back(QString::fromStdWString(image_path.toWString()));

        if (!image_list_path.empty() && image_list_path.exists()) {

            std::ifstream ifs;
            ifs.open(image_list_path.toString(), std::ifstream::in);
            if (ifs.is_open()) {

                std::string line;
                while (std::getline(ifs, line)) {
                    image_list.push_back(QString::fromStdString(line));
                }

                ifs.close();
            }

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

            std::vector<Image> images;
            std::vector<Camera> cameras;

            for (auto &image : image_list) {
                Image img(image);
                if (!project.existImage(img.id()))
                    images.push_back(img);
            }

            for (const auto &camera : project.cameras()) {
                cameras.push_back(camera.second);
            }

            LoadImagesTask image_loader_process(&images, &cameras, camera_type, project.crs());

            connect(&image_loader_process, &LoadImagesTask::imageAdded,
                    [&](int imageId, int cameraId) {
                        Image image = images[imageId];
                        Camera camera = cameras[cameraId];
                        int id_camera = 0;
                        for (const auto &_camera : project.cameras()) {
                            std::string camera_make = _camera.second.make();
                            std::string camera_model = _camera.second.model();
                            if (camera.make().compare(camera_make) == 0 &&
                                camera.model().compare(camera_model) == 0) {
                                id_camera = _camera.first;
                                break;
                            }
                        }
                        if (id_camera == 0)
                            id_camera = project.addCamera(camera);

                        image.setCameraId(id_camera);
                        project.addImage(image);

                        QString crs_proj = project.crs();
                        QString crs_image = image.cameraPose().crs();
                        if (crs_proj.isEmpty() && !crs_image.isEmpty()) {
                            project.setCrs(crs_image);
                        }

                    });

            image_loader_process.run();

        }

        project.save(prj_path);

    } catch (const std::exception &e) {

        tl::printException(e);

        r = true;
    }

    return r;
}

} // namespace graphos