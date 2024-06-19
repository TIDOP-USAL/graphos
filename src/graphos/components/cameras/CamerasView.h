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
        pinhole_1,
        pinhole_2,
        opencv_1,
        opencv_2,
        opencv_fisheye,
        radial_fisheye_1,
        radial_fisheye_2
    };

public:

    /*!
     * \brief Constructor for CamerasView.
     * \param[in] parent The parent widget.
     */
    CamerasView(QWidget *parent) : DialogView(parent) {}

    /*!
     * \brief Destructor for CamerasView.
     */
    ~CamerasView() override = default;

    /*!
     * \brief Retrieves the ID of the active camera.
     * \return The ID of the active camera.
     */
    virtual auto activeCamera() const -> int = 0;

    /*!
     * \brief Sets the active camera.
     * \param[in] id The ID of the camera to set as active.
     */
    virtual void setActiveCamera(int id) = 0;

    /*!
     * \brief Enables or disables camera edition.
     * \param[in] enable Boolean flag indicating whether camera edition should be enabled.
     */
    virtual void enableCameraEdition(bool enable) = 0;

    /*!
     * \brief Adds a camera with the given ID and name.
     * \param[in] cameraId The ID of the camera to add.
     * \param[in] cameraName The name of the camera to add.
     */
    virtual void addCamera(int cameraId, const QString &cameraName) = 0;

    /*!
     * \brief Sets the make of the camera.
     * \param[in] make The make of the camera.
     */
    virtual void setMake(const QString &make) = 0;

    /*!
     * \brief Sets the model of the camera.
     * \param[in] model The model of the camera.
     */
    virtual void setModel(const QString &model) = 0;

    /*!
     * \brief Sets the width of the camera.
     * \param[in] width The width of the camera.
     */
    virtual void setWidth(int width) = 0;

    /*!
     * \brief Sets the height of the camera.
     * \param[in] height The height of the camera.
     */
    virtual void setHeight(int height) = 0;

    /*!
     * \brief Sets the sensor size of the camera.
     * \param[in] sensorSize The sensor size of the camera.
     */
    virtual void setSensorSize(const QString &sensorSize) = 0;

    /*!
     * \brief Sets the focal length of the camera.
     * \param[in] focal The focal length of the camera.
     */
    virtual void setFocal(const QString &focal) = 0;

    /*!
     * \brief Sets the type of the camera.
     * Types:
     * - Radial 1
     * - Radial 2
     * - Pinhole 1
     * - Pinhole 2
     * - OpenCV 1
     * - OpenCV 2
     * - OpenCV Fisheye
     * - Radial Fisheye 1
     * - Radial Fisheye 2
     * \param[in] type The type of the camera.
     */
    virtual void setType(const QString &type) = 0;

    /*!
     * \brief setImages Sets the images associated with the camera.
     * \param[in] images The list of images associated with the camera.
     */
    virtual void setImages(const QStringList &images) = 0;

    // Calibration parameters setters
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

    // Signals for camera changes
    void cameraChange(int);
    void makeChanged(QString);
    void modelChanged(QString);
    void sensorSizeChange(QString);
    void focalChange(QString);
    void typeChange(QString);

    // Signals for calibration import/export
    void calibrationImport(QString, QString);
    void calibrationExport(QString, QString);

    // Calibration parameter change signals
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
