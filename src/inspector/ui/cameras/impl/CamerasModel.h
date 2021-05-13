#ifndef INSPECTOR_CAMERAS_MODEL_H
#define INSPECTOR_CAMERAS_MODEL_H

#include "inspector/ui/cameras/CamerasModel.h"
#include "inspector/core/project.h"

namespace inspector
{

class Image;

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

  int addCamera(const Camera &camera) override;
  int cameraID(const Camera &camera) const override;
  int cameraID(const QString &make, 
               const QString &model) const override;
  Camera camera(int id) const override;
  Camera camera(const QString &make, 
                const QString &model) const override;
  int currentCameraID() const override;
  bool updateCamera(int id, const Camera &camera) override;
  bool removeCamera(int id) override;
  bool removeCamera(const Camera &camera) override;
  QStringList imagesFromCamera(int id) const override;

  camera_iterator begin() override;
  camera_const_iterator begin() const override;
  camera_iterator end() override;
  camera_const_iterator end() const override;

  void save() override;

  //image_iterator imageBegin() override;
  //image_const_iterator imageBegin() const override;
  //image_iterator imageEnd() override;
  //image_const_iterator imageEnd() const override;

public slots:

  void updateCurrentCameraMake(const QString &make) override;
  void updateCurrentCameraModel(const QString &model) override;
  //void updateCurrentCameraWidth(int width) override;
  //void updateCurrentCameraHeight(int height) override;
  void updateCurrentCameraSensorSize(const QString &sensorSize) override;
  void updateCurrentCameraFocal(const QString &focal) override;
  void updateCurrentCameraType(const QString &type) override;
  void updateCurrentCameraCalibCx(double cx) override;
  void updateCurrentCameraCalibCy(double cy) override;
  void updateCurrentCameraCalibF(double f) override;
  void updateCurrentCameraCalibFx(double fx) override;
  void updateCurrentCameraCalibFy(double fy) override;
  void updateCurrentCameraCalibK1(double k1) override;
  void updateCurrentCameraCalibK2(double k2) override;
  void updateCurrentCameraCalibK3(double k3) override;
  void updateCurrentCameraCalibK4(double k4) override;
  void updateCurrentCameraCalibK5(double k5) override;
  void updateCurrentCameraCalibK6(double k6) override;
  void updateCurrentCameraCalibP1(double p1) override;
  void updateCurrentCameraCalibP2(double p2) override;

  void calibrationImport(const QString &file,
                         const QString &format) override;
  void calibrationExport(const QString &file,
                         const QString &format) override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

//

private:

  void updateCalibrationParameter(Calibration::Parameters param, double value);

protected:

  Project *mProject;
  bool bModifiedProject;
  mutable int mActiveCameraId;
  mutable std::map<int, Camera> mCameraCache;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_CAMERAS_MODEL_H
