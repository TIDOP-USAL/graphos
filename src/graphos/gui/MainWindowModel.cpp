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
//#include "graphos/core/sfm/orientationcolmap.h"
#include "graphos/core/ply.h"
#include "graphos/core/project/io/ProjectReader.h"

#include <tidop/rastertools/io/Reader.h>

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
    return QString::fromStdString(mProject->info().name());
}

auto MainWindowModel::projectPath() const -> tl::Path
{
    return mProject->info().projectPath();
}

auto MainWindowModel::images() const -> const ImageRepository &
{
    return mProject->images();
}

//auto MainWindowModel::image(size_t imageId) const -> Image
//{
//    try {
//        return mProject->findImageById(imageId);
//    } catch (...) {
//        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
//    }
//}

auto MainWindowModel::cameras() const -> const CameraRepository &
{
    return mProject->cameras();
}

//auto MainWindowModel::camera(int id) const -> Camera
//{
//    try {
//        return mProject->findCamera(id);
//    } catch (...) {
//        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
//    }
//}

void MainWindowModel::deleteImages(const std::vector<size_t> &imageIds)
{
    try {

        //std::vector<std::string> images;
        auto &image_repo = mProject->images();

        for (auto image_id : imageIds) {
            if (auto image = image_repo.find(image_id)) {
                //images.push_back(image->path().toString());
                image_repo.erase(image_id);
            }
            //images.push_back(mProject->findImageById(image_id).path().toStdString());
            //mProject->removeImage(image_id);
        }

        //std::string reconstruction_path = mProject->;
        //if (!reconstruction_path.empty())
        //    colmapRemoveOrientations(images, reconstruction_path);

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }
}

auto MainWindowModel::readImage(const tl::Path &imagePath) -> QImage
{
    QImage image;

    try {
        
        tl::RasterReader imageReader(imagePath);
        if (imageReader.isOpen()) {

            /// Imagen georeferenciada.
            /// TODO: mostrar coordenadas en la barra de estado
            bool geo = imageReader.isGeoreferenced();
            auto affine = imageReader.georeference();

            cv::Mat bmp;

            tl::DataType data_type = imageReader.dataType();
            //if (data_type == tl::DataType::TL_32F ||
            //    data_type == tl::DataType::TL_64F) {
            //    /// TODO: Aplicar paleta, mapa de sombras, etc, al DTM
            //    bmp = imageReader->read();
            //} else {
                bmp = imageReader.read();
            //}

            image = cvMatToQImage(bmp);

            //imageReader->close();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
    }

    return image;
}

auto MainWindowModel::features() const -> const FeaturesRepository &
{
    return mProject->features();
}

auto MainWindowModel::matches() const -> const MatchingRepository & 
{
    return mProject->matches();
}

auto MainWindowModel::imagePairs(size_t imageId) const -> std::vector<size_t>
{
    std::vector<size_t> pairs;

    if (auto image = mProject->images().find(imageId)) {
        auto image_pairs = mProject->matches().imagePairs(image->path().toString());
        pairs.reserve(image_pairs.size());
        for (auto &pair : image_pairs) {
            pairs.push_back(tl::Path::hash(pair));
        }

    }

    return pairs;
}

auto MainWindowModel::imageIdsWithPairs() const -> std::unordered_set<size_t>
{
    std::unordered_set<size_t> imageIds;
    const auto imageNames = mProject->matches().imagesWithPairs();

    for (const auto &[imageId, image] : mProject->images()) {
        if (imageNames.find(image.path().toUtf8()) != imageNames.end()) {
            imageIds.insert(imageId);
        }
    }

    return imageIds;
}

auto MainWindowModel::sparseModel() const -> tl::Path
{
    return mProject->sparseModel();
}

auto MainWindowModel::isAbsoluteOrientation() const -> bool
{
    return !mProject->info().enuCrs().empty();
}

auto MainWindowModel::poses() const -> const CameraPosesRepository &
{
    return mProject->cameraPoses();
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
    return mProject->meshModel();
}

auto MainWindowModel::dtm() const -> tl::Path
{
    return mProject->dtm();
}

auto MainWindowModel::dsm() const -> tl::Path
{
    return mProject->dsm();
}

auto MainWindowModel::orthophotos() const -> std::map<size_t, tl::Path>
{
    // TODO: Por ahora solo se genera una ortofoto
    std::map<size_t, tl::Path> orthos;
    auto ortho = mProject->orthophoto();
    if (!ortho.empty()) orthos[0] = mProject->orthophoto();
    return orthos;
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
    //return mProject->checkOldVersion(file);
    return false;
}

void MainWindowModel::oldVersionBackup(const tl::Path &file) const
{
    //mProject->oldVersionBak(file);
}

void MainWindowModel::load(const tl::Path &file) const
{
    try {
        ProjectReader reader;
        reader.read(file, *mProject);
        //mProject->load(file);
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
