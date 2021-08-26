/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#include "ProjectModel.h"

namespace graphos
{

ProjectModelImp::ProjectModelImp(Project *project,
                                 QObject *parent)
  : ProjectModel(parent),
    mProject(project),
    //mPrjFile(""),
    bUnsavedChanges(false)
{
  init();
}

ProjectModelImp::~ProjectModelImp()
{

}

QString ProjectModelImp::projectName() const
{
  return mProject->name();
}

QString ProjectModelImp::projectDescription() const
{
  return mProject->description();
}

QString ProjectModelImp::projectFolder() const
{
  return mProject->projectFolder();
}

QString ProjectModelImp::projectPath() const
{
  return mProject->projectPath();
}

QString ProjectModelImp::version() const
{
  return mProject->version();
}

bool ProjectModelImp::checkUnsavedChanges() const
{
  return bUnsavedChanges;
}

bool ProjectModelImp::checkOldVersion(const QString &file) const
{
  return mProject->checkOldVersion(file);
}

void ProjectModelImp::oldVersionBackup(const QString &file) const
{
  mProject->oldVersionBak(file);
}

//void ProjectModelImp::addImage(const Image &img)
//{
//  mProject->addImage(img);
//  bUnsavedChanges = true;
//}

//void ProjectModelImp::deleteImage(const QString &img)
//{
//  mProject->removeImage(img);
//  bUnsavedChanges = true;
//}

//void ProjectModelImp::deleteImage(size_t imgId)
//{
//  mProject->removeImage(imgId);
//  bUnsavedChanges = true;
//}

//const Image ProjectModelImp::findImage(const QString &path) const
//{
//  return mProject->findImage(path);
//}

//const Image ProjectModelImp::findImageById(size_t id) const
//{
//  return mProject->findImageById(id);
//}

//const Image ProjectModelImp::findImageByName(const QString &imgName) const
//{
//  return mProject->findImageByName(imgName);
//}

//size_t ProjectModelImp::findImageId(const QString &path) const
//{
//  return mProject->imageId(path);
//}

//Project::image_const_iterator ProjectModelImp::imageBegin() const
//{
//  return mProject->imageBegin();
//}

//Project::image_const_iterator ProjectModelImp::imageEnd() const
//{
//  return mProject->imageEnd();
//}

//size_t ProjectModelImp::imagesCount() const
//{
//  return mProject->imagesCount();
//}

//void ProjectModelImp::addImages(const QStringList &fileNames)
//{
//  for (const auto &file : fileNames){
//    Image photo(file);

//    ///TODO: Lectura de metadatos EXIF

//    mProject->addImage(photo);
//  }
//}

//void ProjectModelImp::deleteImages(const QStringList &images)
//{
//  for (const auto &image : images){
//    deleteImage(image);
//  }
//}

QString ProjectModelImp::sparseModel() const
{
  return mProject->sparseModel();
}

bool ProjectModelImp::isAbsoluteOriented() const
{
  return !mProject->offset().isEmpty();
}

QString ProjectModelImp::denseModel() const
{
  return mProject->denseModel();
}

QString ProjectModelImp::dtm() const
{
  return mProject->dtmPath();
}

void ProjectModelImp::setProjectName(const QString &name)
{
  mProject->setName(name);
  bUnsavedChanges = true;
}

void ProjectModelImp::setProjectDescription(const QString &description)
{
  mProject->setDescription(description);
  bUnsavedChanges = true;
}

void ProjectModelImp::setProjectFolder(const QString &dir)
{
  mProject->setProjectFolder(dir);
  bUnsavedChanges = true;
}

void ProjectModelImp::setDatabase(const QString &database)
{
  mProject->setDatabase(database);
  bUnsavedChanges = true;
}

void ProjectModelImp::setSparseModel(const QString &sparseModel)
{
  mProject->setSparseModel(sparseModel);
  bUnsavedChanges = true;
}

void ProjectModelImp::setDenseModel(const QString &denseModel)
{
  mProject->setDenseModel(denseModel);
  bUnsavedChanges = true;
}

void ProjectModelImp::load(const QString &file)
{
  //mPrjFile = file;
  mProject->load(file);
  bUnsavedChanges = false;
}

void ProjectModelImp::save()
{
  saveAs(mProject->projectPath());
}

void ProjectModelImp::saveAs(const QString &file)
{
  //mPrjFile = file;
  mProject->save(file);

  bUnsavedChanges = false;
}

void ProjectModelImp::init()
{

}

void ProjectModelImp::clear()
{
  mProject->clear();
  //mPrjFile = "";
}

} // namespace photomatch
