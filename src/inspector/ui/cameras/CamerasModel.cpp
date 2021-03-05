#include "CamerasModel.h"

#include <tidop/core/messages.h>

namespace inspector
{

namespace ui
{

CamerasModelImp::CamerasModelImp(Project *project,
                                 QObject *parent)
  : CamerasModel(parent),
    mProject(project)
{
  init();
}

int CamerasModelImp::addCamera(const Camera &camera)
{
  return mProject->addCamera(camera);
}

int CamerasModelImp::cameraID(const Camera &camera) const
{
  return cameraID(camera.make(), camera.model());
}

int CamerasModelImp::cameraID(const QString &make, const QString &model) const
{
  int id_camera = 0;
  for (auto it = begin(); it != end(); it++){
    QString camera_make = it->second.make();
    QString camera_model = it->second.model();
    if (make.compare(camera_make) == 0 &&
        model.compare(camera_model) == 0){
      id_camera = it->first;
      break;
    }
  }
  return id_camera;
}

Camera CamerasModelImp::camera(int id) const
{
  return mProject->findCamera(id);
}

Camera CamerasModelImp::camera(const QString &make, const QString &model) const
{
  int camera_id = cameraID(make, model);
  return camera(camera_id);
}

bool CamerasModelImp::updateCamera(int id, const Camera &camera)
{
  return mProject->updateCamera(id, camera);
}

bool CamerasModelImp::removeCamera(int id)
{
  return mProject->removeCamera(id);
}

bool CamerasModelImp::removeCamera(const Camera &camera)
{
  int id_camera = cameraID(camera.make(), camera.model());
  return mProject->removeCamera(id_camera);
}

CamerasModel::camera_iterator CamerasModelImp::begin()
{
  return mProject->cameraBegin();
}

CamerasModel::camera_const_iterator CamerasModelImp::begin() const
{
  return mProject->cameraBegin();
}

CamerasModel::camera_iterator CamerasModelImp::end()
{
  return mProject->cameraEnd();
}

CamerasModel::camera_const_iterator CamerasModelImp::end() const
{
  return mProject->cameraEnd();
}

CamerasModel::image_iterator CamerasModelImp::imageBegin()
{
  return mProject->imageBegin();
}

CamerasModel::image_const_iterator CamerasModelImp::imageBegin() const
{
  return mProject->imageEnd();
}

CamerasModel::image_iterator CamerasModelImp::imageEnd()
{
  return mProject->imageEnd();
}

CamerasModel::image_const_iterator CamerasModelImp::imageEnd() const
{
  return mProject->imageEnd();
}

void CamerasModelImp::init()
{

}

void CamerasModelImp::clear()
{
  for (auto it = begin(); it != end();){
    removeCamera(it->first);
    it = begin();
  }
}

} // namespace ui

} // namespace inspector


