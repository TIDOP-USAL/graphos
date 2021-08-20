#ifndef GRAPHOS_CAMERAS_MODEL_INTERFACE_H
#define GRAPHOS_CAMERAS_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class Camera;
//class Image;

namespace ui
{

class CamerasModel
  : public Model
{

  Q_OBJECT

public:

  typedef std::map<int, Camera>::iterator camera_iterator;
  typedef std::map<int, Camera>::const_iterator camera_const_iterator;
  //typedef std::vector<Image>::iterator image_iterator;
  //typedef std::vector<Image>::const_iterator image_const_iterator;

public:

  CamerasModel(QObject *parent = nullptr) : Model(parent) {}
  virtual ~CamerasModel() override = default;

  virtual int addCamera(const Camera &camera) = 0;
  virtual int cameraID(const Camera &camera) const = 0;
  virtual int cameraID(const QString &make, 
                       const QString &model) const = 0;
  virtual Camera camera(int id) const = 0;
  virtual Camera camera(const QString &make, const QString &model) const = 0;
  virtual int currentCameraID() const = 0;
  virtual bool updateCamera(int id, const Camera &camera) = 0;

  virtual bool removeCamera(int id) = 0;
  virtual bool removeCamera(const Camera &camera) = 0;
  virtual QStringList imagesFromCamera(int id) const = 0;

  virtual camera_iterator begin() = 0;
  virtual camera_const_iterator begin() const = 0;
  virtual camera_iterator end() = 0;
  virtual camera_const_iterator end() const = 0;

  virtual void save() = 0;

  //virtual image_iterator imageBegin() = 0;
  //virtual image_const_iterator imageBegin() const = 0;
  //virtual image_iterator imageEnd() = 0;
  //virtual image_const_iterator imageEnd() const = 0;

public slots:

  virtual void updateCurrentCameraMake(const QString &make) = 0;
  virtual void updateCurrentCameraModel(const QString &model) = 0;
  //virtual void updateCurrentCameraWidth(int width) = 0;
  //virtual void updateCurrentCameraHeight(int height) = 0;
  virtual void updateCurrentCameraSensorSize(const QString &sensorSize) = 0;
  virtual void updateCurrentCameraFocal(const QString &focal) = 0;
  virtual void updateCurrentCameraType(const QString &type) = 0;
  virtual void updateCurrentCameraCalibCx(double cx) = 0;
  virtual void updateCurrentCameraCalibCy(double cy) = 0;
  virtual void updateCurrentCameraCalibF(double f) = 0;
  virtual void updateCurrentCameraCalibFx(double fx) = 0;
  virtual void updateCurrentCameraCalibFy(double fy) = 0;
  virtual void updateCurrentCameraCalibK1(double k1) = 0;
  virtual void updateCurrentCameraCalibK2(double k2) = 0;
  virtual void updateCurrentCameraCalibK3(double k3) = 0;
  virtual void updateCurrentCameraCalibK4(double k4) = 0;
  virtual void updateCurrentCameraCalibK5(double k5) = 0;
  virtual void updateCurrentCameraCalibK6(double k6) = 0;
  virtual void updateCurrentCameraCalibP1(double p1) = 0;
  virtual void updateCurrentCameraCalibP2(double p2) = 0;

  virtual void calibrationImport(const QString &file,
                                 const QString &format) = 0;
  virtual void calibrationExport(const QString &file,
                                 const QString &format) = 0;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_CAMERAS_MODEL_INTERFACE_H
