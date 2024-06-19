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

#ifndef GRAPHOS_CAMERAS_MODEL_INTERFACE_H
#define GRAPHOS_CAMERAS_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"

namespace graphos
{

class Camera;


/*!
 * \brief The CamerasModel class provides an interface for accessing and managing camera data.
 */
class CamerasModel
  : public Model
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor for CamerasModel.
     * \param[in] parent The parent QObject (default is nullptr).
     */
    CamerasModel(QObject *parent = nullptr) : Model(parent) {}

    /*!
     * \brief Destructor for CamerasModel.
     */
    ~CamerasModel() override = default;

    /*!
     * \brief Retrieves a map of all cameras with their IDs.
     * \return A constant reference to a map of cameras with their IDs.
     */
    virtual auto cameras() const -> const std::map<int, Camera>& = 0;

    /*!
     * \brief addCamera Adds a camera to the model.
     * \param[in] camera The camera to add.
     * \return The ID assigned to the added camera.
     */
    virtual auto addCamera(const Camera &camera) -> int = 0;

    /*!
     * \brief Retrieves the ID of a camera.
     * \param[in] camera The camera to retrieve the ID for.
     * \return The ID of the camera.
     */
    virtual auto cameraID(const Camera &camera) const -> int = 0;

    /*!
     * \brief Retrieves the ID of a camera by its make and model.
     * \param[in] make The make of the camera.
     * \param[in] model The model of the camera.
     * \return The ID of the camera.
     */
    virtual auto cameraID(const QString &make,
                          const QString &model) const -> int = 0;

    /*!
     * \brief Retrieves a camera by its ID.
     * \param[in] id The ID of the camera to retrieve.
     * \return The camera corresponding to the given ID.
     */
    virtual auto camera(int id) const -> Camera = 0;

    /*!
     * \brief Retrieves a camera by its make and model.
     * \param[in] make The make of the camera.
     * \param[in] model The model of the camera.
     * \return The camera corresponding to the given make and model.
     */
    virtual auto camera(const QString &make,
                        const QString &model) const -> Camera = 0;

    /*!
     * \brief Retrieves the ID of the current active camera.
     * \return The ID of the current active camera.
     */
    virtual auto currentCameraID() const -> int = 0;

    /*!
     * \brief Updates the data for a camera.
     * \param[in] id The ID of the camera to update.
     * \param[in] camera The updated camera data.
     * \return True if the update was successful, otherwise false.
     */
    virtual auto updateCamera(int id,
                              const Camera &camera) -> bool = 0;

    /*!
     * \brief Removes a camera from the model by its ID.
     * \param[in] id The ID of the camera to remove.
     * \return True if the removal was successful, otherwise false.
     */
    virtual auto removeCamera(int id) -> bool = 0;

    /*!
     * \brief Removes a camera from the model.
     * \param[in] camera The camera to remove.
     * \return True if the removal was successful, otherwise false.
     */
    virtual auto removeCamera(const Camera &camera) -> bool = 0;

    /*!
     * \brief Retrieves a list of image filenames associated with a camera.
     * \param[in] id The ID of the camera.
     * \return A list of image filenames associated with the camera.
     */
    virtual auto imagesFromCamera(int id) const -> QStringList = 0;

    /*!
     * \brief Checks if the model has been modified.
     * \return True if the model has been modified, otherwise false.
     */
    virtual auto modified() -> bool = 0;

    /*!
     * \brief save Saves the changes made to the model.
     */
    virtual void save() = 0;

public slots:

    // Methods to update current camera parameters
    virtual void updateCurrentCameraMake(const QString &make) = 0;
    virtual void updateCurrentCameraModel(const QString &model) = 0;
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

    // Methods for calibration import/export
    virtual void calibrationImport(const QString &file,
                                   const QString &format) = 0;
    virtual void calibrationExport(const QString &file,
                                   const QString &format) = 0;
};

} // namespace graphos

#endif // GRAPHOS_CAMERAS_MODEL_INTERFACE_H
