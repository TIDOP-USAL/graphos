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

#include "MainWindowModel.h"

#include "graphos/core/utils.h"
#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/ply.h"

#include <tidop/img/imgreader.h>

#include <QSettings>

namespace graphos
{


MainWindowModel::MainWindowModel(Project *project)
  : mProject(project),
    mSettings(new QSettings(QSettings::IniFormat,
              QSettings::UserScope,
              qApp->organizationName(),
              qApp->applicationName()))/*,
  bUnsavedChanges(false)*/
{
    this->init();
}

MainWindowModel::~MainWindowModel()
{
    if (mSettings) {
        delete mSettings;
        mSettings = nullptr;
    }
}

auto MainWindowModel::projectName() const -> QString
{
    return mProject->name();
}

auto MainWindowModel::projectPath() const -> tl::Path
{
    return mProject->projectPath();
}

auto MainWindowModel::images() const -> const std::unordered_map<size_t, Image> &
{
    return mProject->images();
}

auto MainWindowModel::image(size_t imageId) const -> Image
{
    try {
        return mProject->findImageById(imageId);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }
}

auto MainWindowModel::cameras() const -> const std::map<int, Camera> &
{
    return mProject->cameras();
}

auto MainWindowModel::camera(int id) const -> Camera
{
    try {
        return mProject->findCamera(id);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }
}

void MainWindowModel::deleteImages(const std::vector<size_t> &imageIds)
{
    try {

        std::vector<std::string> images;

        for (auto imageId : imageIds) {
            images.push_back(mProject->findImageById(imageId).path().toStdString());
            mProject->removeImage(imageId);
        }

        std::string reconstruction_path = mProject->reconstructionPath().toString();
        if (!reconstruction_path.empty())
            colmapRemoveOrientations(images, reconstruction_path);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }
}

auto MainWindowModel::readImage(const tl::Path &imagePath) -> QImage
{
    QImage image;

    try {
        
        std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(imagePath);
        imageReader->open();
        if (imageReader->isOpen()) {

            /// Imagen georeferenciada.
            /// TODO: mostrar coordenadas en la barra de estado
            bool geo = imageReader->isGeoreferenced();
            auto affine = imageReader->georeference();

            cv::Mat bmp;

            tl::DataType data_type = imageReader->dataType();
            //if (data_type == tl::DataType::TL_32F ||
            //    data_type == tl::DataType::TL_64F) {
            //    /// TODO: Aplicar paleta, mapa de sombras, etc, al DTM
            //    bmp = imageReader->read();
            //} else {
                bmp = imageReader->read();
            //}

            image = cvMatToQImage(bmp);

            imageReader->close();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }

    return image;
}

auto MainWindowModel::features() const -> const std::unordered_map<size_t, QString> &
{
    return mProject->features();
}

auto MainWindowModel::imagePairs(size_t imageId) const -> std::vector<size_t>
{
    std::vector<size_t> image_pairs = mProject->matchesPairs(imageId);
    return image_pairs;
}

auto MainWindowModel::sparseModel() const -> tl::Path
{
    return mProject->sparseModel();
}

auto MainWindowModel::isAbsoluteOrientation() const -> bool
{
    return !mProject->enuCrs().isEmpty();
}

auto MainWindowModel::poses() const -> const std::unordered_map<size_t, CameraPose> &
{
    return mProject->poses();
}

auto MainWindowModel::transform() const -> tl::Matrix<double, 4, 4>
{
    return mProject->transform();
}

auto MainWindowModel::denseModel() const -> tl::Path
{
    return mProject->denseModel();
}

auto MainWindowModel::mesh() const -> tl::Path
{
    return mProject->meshPath();
}

auto MainWindowModel::dtm() const -> tl::Path
{
    return mProject->dem().dtmPath;
}

auto MainWindowModel::dsm() const -> tl::Path
{
    return mProject->dem().dsmPath;
}

auto MainWindowModel::orthophotos() const -> const std::map<size_t, OrthophotoData>&
{
    return mProject->orthophotos();
}

auto MainWindowModel::graphicViewerBackgroundColor() const -> QString
{
    return mSettings->value("ImageViewer/BackgroundColor", "#dcdcdc").toString();
}

//bool MainWindowModel::checkUnsavedChanges() const
//{
//  return bUnsavedChanges;
//}

auto MainWindowModel::checkOldVersion(const tl::Path &file) const -> bool
{
    return mProject->checkOldVersion(file);
}

void MainWindowModel::oldVersionBackup(const tl::Path &file) const
{
    mProject->oldVersionBak(file);
}

void MainWindowModel::load(const tl::Path &file) const
{
    try {
        mProject->load(file);
    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Exception caught when loading the project");
    }
}

//void MainWindowModel::save()
//{
//  try {
//
//    saveAs(mProject->projectPath());
//
//  } catch (...) {
//    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
//  }
//}

//void MainWindowModel::saveAs(const tl::Path &file)
//{
//
//  mProject->save(file);
//
//  bUnsavedChanges = false;
//}

void MainWindowModel::init()
{

}

void MainWindowModel::clear()
{
    mProject->clear();
}


} // namespace graphos



