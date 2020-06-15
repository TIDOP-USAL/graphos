#include "ProjectModel.h"

namespace inspector
{

namespace ui
{

ProjectModelImp::ProjectModelImp(Project *project,
                                 QObject *parent)
  : ProjectModel(parent),
    mProject(project),
    mPrjFile(""),
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
  return mPrjFile;
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

QString ProjectModelImp::denseModel() const
{
  return mProject->denseModel();
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
  mPrjFile = file;
  mProject->load(file);
  bUnsavedChanges = false;
}

void ProjectModelImp::save()
{
  saveAs(mPrjFile);
}

void ProjectModelImp::saveAs(const QString &file)
{
  mPrjFile = file;
  mProject->save(file);

  bUnsavedChanges = false;
}

void ProjectModelImp::init()
{

}

void ProjectModelImp::clear()
{
  mProject->clear();
  mPrjFile = "";
}

} // namespace ui

} // namespace photomatch
