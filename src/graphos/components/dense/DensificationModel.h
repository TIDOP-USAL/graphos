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

#pragma once

#include "graphos/core/mvp.h"

#include <unordered_map>

#include <tidop/core/base/Path.h>

#include "graphos/core/dense/DensificationReport.h"

namespace graphos
{

class ImageRepository;
class CameraRepository;
class CameraPosesRepository;
class DensificationProperties;
class GroundPoint;


/*!
 * \brief The DensificationModel class provides an interface for accessing densification-related data.
 */
class DensificationModel
  : public Model
{

    Q_OBJECT

public:

    /*!
     * \brief Constructor for DensificationModel.
     * \param[in] parent The parent QObject (default is nullptr).
     */
    DensificationModel(QObject *parent = nullptr) : Model(parent) {}

    /*!
     * \brief Destructor for DensificationModel.
     */
    ~DensificationModel() override = default;

    /*!
     * \brief Retrieves the densification object.
     * \return A shared pointer to the densification object.
     */
    virtual auto densificationConfig() const -> std::shared_ptr<DensificationProperties> = 0;

    /*!
     * \brief Retrieves the project folder path.
     * \return The project folder path.
     */
    virtual auto projectFolder() const -> tl::Path = 0;

    /*!
     * \brief Retrieves the path to the reconstruction.
     * \return The reconstruction path.
     */
    //virtual auto reconstructionPath() const -> tl::Path = 0;

    /*!
     * \brief Retrieves the path to the database.
     * \return The path to the database.
     */
    virtual auto database() const -> tl::Path = 0;

    /*!
     * \brief Checks if CUDA is being used.
     * \return True if CUDA is being used, otherwise false.
     */
    virtual auto useCuda() const -> bool = 0;

    /*!
     * \brief Checks if a dense model exists.
     * \return True if a dense model exists, otherwise false.
     */
    virtual auto existDenseModel() const -> bool = 0;

    /*!
     * \brief Retrieves a map of images.
     * \return A constant reference to a map of images.
     */
    virtual auto images() const -> const ImageRepository & = 0;

    /*!
     * \brief Retrieves a map of cameras.
     * \return A constant reference to a map of cameras.
     */
    virtual auto cameras() const -> const CameraRepository & = 0;

    /*!
     * \brief Retrieves a map of camera poses.
     * \return A constant reference to a map of camera poses.
     */
    virtual auto cameraPoses() const -> const CameraPosesRepository & = 0;

    /*!
     * \brief Retrieves a vector of ground points.
     * \return A vector of ground points.
     */
    virtual auto groundPoints() const -> std::vector<GroundPoint> = 0;

    virtual void cleanProject() = 0;

public slots:

    /*!
     * \brief Sets the densification object.
     * \param[in] densification The densification object to set.
     */
    virtual void setDensificationConfig(std::shared_ptr<DensificationProperties> config) = 0;

    /*!
     * \brief Sets the path to the dense model.
     * \param[in] denseModel The path to the dense model.
     */
    virtual void setDenseModel(tl::Path denseModel) = 0;

    /*!
     * \brief Sets the dense report.
     * \param[in] report The dense report to set.
     */
    virtual void setDensificationReport(DensificationReport report) = 0;

};

} // End namespace graphos