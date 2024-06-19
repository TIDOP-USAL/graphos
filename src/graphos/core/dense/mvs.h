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

#ifndef GRAPHOS_CORE_DENSE_MVS_H
#define GRAPHOS_CORE_DENSE_MVS_H

#include "config_graphos.h"

#include "graphos/core/dense/dense.h"

#include <tidop/core/path.h>

#include <QString>
#include <unordered_map>

namespace colmap
{
struct FeatureKeypoint;
}

namespace graphos
{


/*!
 * \brief MVS properties for dense reconstruction.
 *
 * This class represents properties related to Multi-View Stereo (MVS) for dense reconstruction.
 * It allows configuring parameters such as resolution levels, number of views, and estimation options.
 *
 * For more information on Multi-View Stereo (MVS), refer to: https://github.com/cdcseacave/openMVS
 */
class Mvs
  : public Densification
{

public:

    /*!
     * \brief Default constructor.
     * Constructs an Mvs object with default values.
     */
    Mvs();

    /*!
     * \brief Parameterized constructor.
     * Constructs an Mvs object with specified parameters.
     * \param[in] resolutionLevel The resolution level.
     * \param[in] minResolution The minimum resolution.
     * \param[in] maxResolution The maximum resolution.
     * \param[in] numberViews The number of views.
     * \param[in] numberViewsFuse The number of views to fuse.
     */
    Mvs(int resolutionLevel,
        int minResolution,
        int maxResolution,
        int numberViews,
        int numberViewsFuse);

    /*!
     * \brief Copy constructor.
     * Constructs a new Mvs object by copying from another Mvs object.
     * \param[in] mvs The Mvs object to copy from.
     */
    Mvs(const Mvs &mvs);

    /*!
     * \brief Move constructor.
     * Constructs a new Mvs object by moving the resources of another Mvs object.
     * \param[in] mvs The Mvs object to move from.
     */
    Mvs(Mvs &&mvs) noexcept;

    /*!
     * \brief Destructor.
     */
    ~Mvs() override = default;

    /*!
     * \brief Copy assignment operator.
     * Assigns values from another Mvs object to this object.
     * \param[in] mvs The Mvs object to assign from.
     * \return Reference to the assigned Mvs object.
     */
    auto operator =(const Mvs& mvs) -> Mvs&;

    /*!
     * \brief Move assignment operator.
     * Moves the resources from another Mvs object to this object.
     * \param[in] mvs The Mvs object to move from.
     * \return Reference to the moved Mvs object.
     */
    auto operator =(Mvs&& mvs) noexcept -> Mvs&;


// Getter methods

    /*!
     * \brief Get the resolution level.
     * \return The resolution level.
     */
    virtual auto resolutionLevel() const -> int;

    /*!
     * \brief Get the minimum resolution.
     * \return The minimum resolution.
     */
    virtual auto minResolution() const -> int;

    /*!
     * \brief Get the maximum resolution.
     * \return The maximum resolution.
     */
    virtual auto maxResolution() const -> int;

    /*!
     * \brief Get the number of views.
     * \return The number of views.
     */
    virtual auto numberViews() const -> int;

    /*!
     * \brief Get the number of views to fuse.
     * \return The number of views to fuse.
     */
    virtual auto numberViewsFuse() const -> int;

    /*!
     * \brief Check if color estimation is enabled.
     * \return True if color estimation is enabled, false otherwise.
     */
    virtual auto estimateColors() const -> bool;

    /*!
     * \brief Check if normals estimation is enabled.
     * \return True if normals estimation is enabled, false otherwise.
     */
    virtual auto estimateNormals() const -> bool;

// Setter methods

    /*!
     * \brief Set the resolution level.
     * \param[in] resolutionLevel The resolution level to set.
     */
    virtual void setResolutionLevel(int resolutionLevel);

    /*!
     * \brief Set the minimum resolution.
     * \param[in] minResolution The minimum resolution to set.
     */
    virtual void setMinResolution(int minResolution);

    /*!
     * \brief Set the maximum resolution.
     * \param[in] maxResolution The maximum resolution to set.
     */
    virtual void setMaxResolution(int maxResolution);

    /*!
     * \brief Set the number of views.
     * \param[in] numberViews The number of views to set.
     */
    virtual void setNumberViews(int numberViews);

    /*!
     * \brief Set the number of views to fuse.
     * \param[in] numberViewsFuse The number of views to fuse.
     */
    virtual void setNumberViewsFuse(int numberViewsFuse);

    /*!
     * \brief Set whether color estimation is enabled.
     * \param[in] estimateColors True to enable color estimation, false otherwise.
     */
    virtual void setEstimateColors(bool estimateColors);

    /*!
     * \brief Set whether normals estimation is enabled.
     * \param[in] estimateNormals True to enable normals estimation, false otherwise.
     */
    virtual void setEstimateNormals(bool estimateNormals);

// Densification interface methods

public:

    void reset() override;
    auto name() const -> QString override;

private:

    int mResolutionLevel;     //!< The resolution level.
    int mMinResolution;       //!< The minimum resolution.
    int mMaxResolution;       //!< The maximum resolution.
    int mNumberViews;         //!< The number of views.
    int mNumberViewsFuse;     //!< The number of views to fuse.
    bool mEstimateColors;     //!< Flag indicating whether color estimation is enabled.
    bool mEstimateNormals;    //!< Flag indicating whether normal estimation is enabled.

};




/*!
 * \brief Class for Multi-View Stereo (MVS) densification.
 *
 * This class represents a densifier using Multi-View Stereo (MVS) reconstruction.
 * It inherits properties related to MVS reconstruction from Mvs and functionality
 * for densification from DensifierBase.
 */
class MvsDensifier
  : public Mvs,
    public DensifierBase
{

public:

    /*!
     * \brief Constructor for MVS densifier.
     * Constructs an MvsDensifier object with provided parameters.
     * \param[in] images Images used for densification.
     * \param[in] cameras Cameras associated with the images.
     * \param[in] poses Camera poses associated with the images.
     * \param[in] groundPoints Ground points
     * \param[in] outputPath The output path for storing densification results.
     * \param[in] database The path to the database.
     * \param[in] cuda Flag indicating whether to use CUDA for computation (default is false).
     * \param[in] autoSegmentation Flag indicating whether to enable auto-segmentation (default is false).
     */
    MvsDensifier(const std::unordered_map<size_t, Image> &images,
                 const std::map<int, Camera> &cameras,
                 const std::unordered_map<size_t, CameraPose> &poses,
                 const std::vector<GroundPoint> &groundPoints,
                 const tl::Path &outputPath,
                 tl::Path database,
                 bool cuda = false,
                 bool autoSegmentation = false);

    ~MvsDensifier() override;

    TL_DISABLE_COPY(MvsDensifier)
    TL_DISABLE_MOVE(MvsDensifier)

private:

    void clearTemporalFiles() const;
    void exportCameras(const std::map<int, Undistort> &undistortMap, 
                       const tl::Path &colmapSparsePath) const;
    void exportImages(const std::unordered_map<size_t, uint32_t> &graphosToColmapImageIds,
                      const std::unordered_map<size_t, std::vector<colmap::FeatureKeypoint>> &keypoints,
                      const tl::Path &colmapSparsePath) const;
    void exportPoints(const std::unordered_map<size_t, uint32_t> &graphosToColmapImageIds,
                      const tl::Path &colmapSparsePath) const;
    void exportToColmap() const;
    void writeNvmFile() const;
    void exportToMvs() const;
    void densify();

// TaskBase

protected:

    /*!
     * \brief Executes the densification task.
     * Overrides the execute method from the TaskBase class.
     * \param progressBar Pointer to a progress bar for tracking progress (default is nullptr).
     */
    void execute(tl::Progress *progressBar = nullptr) override;

private:

    tl::Path mDatabase;
    bool mAutoSegmentation;

};


} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_MVS_H
