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

#ifndef GRAPHOS_CAMERAS_VIEW_INTERFACE_H
#define GRAPHOS_CAMERAS_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"


namespace graphos
{

class CamerasView
  : public DialogView
{

  Q_OBJECT

public:

  enum cameraType
  {
    radial_1,
    radial_2,
    /*radial_3,*/
    pinhole_1,
    pinhole_2,
    opencv_1,
    opencv_2,
    opencv_fisheye,
    radial_fisheye_1,
    radial_fisheye_2
  };

public:

  CamerasView(QWidget *parent) : DialogView(parent) {}
  virtual ~CamerasView() {}

  virtual int activeCamera() const = 0;
  virtual void setActiveCamera(int id) = 0;

  virtual void enableCameraEdition(bool enable) = 0;
  virtual void addCamera(int cameraId, const QString &cameraName) = 0;
  virtual void setMake(const QString &make) = 0;
  virtual void setModel(const QString &model) = 0;
  virtual void setWidth(int width) = 0;
  virtual void setHeight(int height) = 0;
  virtual void setSensorSize(const QString &sensorSize) = 0;
  virtual void setFocal(const QString &focal) = 0;
  virtual void setType(const QString &type) = 0;
  virtual void setImages(const QStringList &images) = 0;

  virtual void setCalibCx(double cx) = 0;
  virtual void setCalibCy(double cy) = 0;
  virtual void setCalibF(double f) = 0;
  virtual void setCalibFx(double fx) = 0;
  virtual void setCalibFy(double fy) = 0;
  virtual void setCalibK1(double k1) = 0;
  virtual void setCalibK2(double k2) = 0;
  virtual void setCalibK3(double k3) = 0;
  virtual void setCalibK4(double k4) = 0;
  virtual void setCalibK5(double k5) = 0;
  virtual void setCalibK6(double k6) = 0;
  virtual void setCalibP1(double p1) = 0;
  virtual void setCalibP2(double p2) = 0;

signals:

  void cameraChange(int);
  void makeChanged(QString);
  void modelChanged(QString);
  void sensorSizeChange(QString);
  void focalChange(QString);
  void typeChange(QString);

  void calibrationImport(QString, QString);
  void calibrationExport(QString, QString);
  //void fixCalibration(bool);

  void calibCxChange(double);
  void calibCyChange(double);
  void calibFChange(double);
  void calibFxChange(double);
  void calibFyChange(double);
  void calibK1Change(double);
  void calibK2Change(double);
  void calibK3Change(double);
  void calibK4Change(double);
  void calibK5Change(double);
  void calibK6Change(double);
  void calibP1Change(double);
  void calibP2Change(double);

protected slots:

  virtual void onSelectionChanged() = 0;

};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_VIEW_INTERFACE_H
