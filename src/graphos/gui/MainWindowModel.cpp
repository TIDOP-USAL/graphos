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

#include <tidop/img/imgreader.h>
//#include <tidop/img/metadata.h>
//#include <tidop/math/angles.h>





namespace graphos
{

MainWindowModel::MainWindowModel(Project *project)
  : mProject(project),
    bUnsavedChanges(false)
{
  this->init();
}

QString MainWindowModel::projectName() const
{
  return mProject->name();
}

QString MainWindowModel::projectPath() const
{
  return mProject->projectPath();
}

const std::unordered_map<size_t, Image> &MainWindowModel::images() const
{
  return mProject->images();
}

Image MainWindowModel::image(size_t imageId) const
{
  try {
    return mProject->findImageById(imageId);
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

  QString reconstruction_path = mProject->reconstructionPath();
  if (!reconstruction_path.isEmpty())
    colmapRemoveOrientations(images, reconstruction_path.toStdString());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }
}

QImage MainWindowModel::readImage(size_t imageId)
{
  QImage image;

  try {

    std::unique_ptr<tl::ImageReader> imageReader = tl::ImageReaderFactory::create(this->image(imageId).path().toStdWString());
    imageReader->open();
    if (imageReader->isOpen()) {

      /// Imagen georeferenciada.
      /// TODO: mostrar coordenadas en la barra de estado
      bool geo = imageReader->isGeoreferenced();

      cv::Mat bmp;

      tl::DataType data_type = imageReader->dataType();
      if (data_type == tl::DataType::TL_32F ||
          data_type == tl::DataType::TL_64F) {
        /// TODO: Aplicar paleta, mapa de sombras, etc, al DTM
      } else {
        bmp = imageReader->read();
      }

      image = cvMatToQImage(bmp);

      imageReader->close();
    }

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }

  return image;
}

const std::unordered_map<size_t, QString> &MainWindowModel::features() const
{
  return mProject->features();
}

std::vector<size_t> MainWindowModel::imagePairs(size_t imageId) const
{
  std::vector<size_t> image_pairs = mProject->matchesPairs(imageId);
  return image_pairs;
}

QString MainWindowModel::sparseModel() const
{
  return mProject->sparseModel();
}

bool MainWindowModel::isAbsoluteOrientation() const
{
  return !mProject->offset().isEmpty();
}

const std::unordered_map<size_t, CameraPose> &MainWindowModel::poses() const
{
  return mProject->poses();
}

QString MainWindowModel::denseModel() const
{
  return mProject->denseModel();
}

bool MainWindowModel::checkUnsavedChanges() const
{
  return bUnsavedChanges;
}

bool MainWindowModel::checkOldVersion(const QString &file) const
{
  return mProject->checkOldVersion(file);
}

void MainWindowModel::oldVersionBackup(const QString &file) const
{
  mProject->oldVersionBak(file);
}

void MainWindowModel::load(const QString &file)
{
  mProject->load(file);
  bUnsavedChanges = false;
}

void MainWindowModel::save()
{
  try {

    saveAs(mProject->projectPath());

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Catched exception");
  }
}

void MainWindowModel::saveAs(const QString &file)
{
  mProject->save(file);

  bUnsavedChanges = false;
}

void MainWindowModel::init()
{

}

void MainWindowModel::clear()
{
  mProject->clear();
}


} // namespace graphos



