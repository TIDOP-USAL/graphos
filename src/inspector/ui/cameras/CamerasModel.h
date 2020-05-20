#ifndef INSPECTOR_CAMERAS_MODEL_H
#define INSPECTOR_CAMERAS_MODEL_H

#include "inspector/ui/cameras/Cameras.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class CamerasModelImp
  : public CamerasModel
{

  Q_OBJECT

public:

  CamerasModelImp(Project *project,
                  QObject *parent = nullptr);
  ~CamerasModelImp() override = default;

// CamerasModel interface

public:

  int addCamera(const QString &image) override;
  int addCamera(const Camera &camera) override;
  int cameraID(const Camera &camera) const override;
  int cameraID(const QString &make, const QString &model) const override;
  Camera camera(int id) const override;
  Camera camera(const QString &make, const QString &model) const override;
  bool updateCamera(int id, const Camera &camera) override;
  bool removeCamera(int id) override;
  bool removeCamera(const Camera &camera) override;

  camera_iterator begin() override;
  camera_const_iterator begin() const override;
  camera_iterator end() override;
  camera_const_iterator end() const override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_MODEL_H
