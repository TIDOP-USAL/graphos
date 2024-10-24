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

#ifndef GRAPHOS_CAMERAS_MODEL_H
#define GRAPHOS_CAMERAS_MODEL_H

#include "graphos/components/cameras/CamerasModel.h"
#include "graphos/core/project.h"

namespace graphos
{

class Image;

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

    auto cameras() const -> const std::map<int, Camera> & override;
    auto addCamera(const Camera &camera) -> int override;
    auto cameraID(const Camera &camera) const -> int override;
    auto cameraID(const QString &make,
                  const QString &model) const -> int override;
    auto camera(int id) const -> Camera override;
    auto camera(const QString &make,
                const QString &model) const -> Camera override;
    auto currentCameraID() const -> int override;
    auto updateCamera(int id, const Camera &camera) -> bool override;
    auto removeCamera(int id) -> bool override;
    auto removeCamera(const Camera &camera) -> bool override;
    auto imagesFromCamera(int id) const -> QStringList override;

    void save() override;

    auto modified() -> bool override;

public slots:

    void updateCurrentCameraMake(const QString &make) override;
    void updateCurrentCameraModel(const QString &model) override;
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

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

private:

    void updateCalibrationParameter(Calibration::Parameters param,
                                    double value);

protected:

    Project *mProject;
    bool mModifiedProject;
    mutable int mActiveCameraId;
    mutable std::map<int, Camera> mCameraCache;
};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_MODEL_H
