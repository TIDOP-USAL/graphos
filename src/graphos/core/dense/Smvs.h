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

#ifndef GRAPHOS_CORE_DENSE_SMVS_H
#define GRAPHOS_CORE_DENSE_SMVS_H

#include "config_graphos.h"

#include "graphos/core/dense/dense.h"

#include <tidop/core/path.h>

#include <QString>

#include <unordered_map>

namespace graphos
{


/*!
 * \brief Shading-aware Multi-view Stereo.
 *
 * Shading-aware Multi-view Stereo (SMVS) is a reconstruction method that incorporates shading-based
 * optimization and semi-global matching (SGM) to generate accurate 3D reconstructions from multiple
 * images. This class represents properties related to SMVS reconstruction.
 *
 * References:
 * - Paper: http://www.kalyans.org/research/2016/ShadingAwareMVS_ECCV16.pdf
 * - GitHub Repository: https://github.com/flanggut/smvs
 */
class Smvs
  : public Densification
{

public:

    /*!
     * \brief Default constructor for SMVS properties.
     * Constructs an Smvs object with default values.
     */
    Smvs();

    /*!
     * \brief Parameterized constructor for SMVS properties.
     * Constructs an Smvs object with provided parameters.
     * \param inputImageScale The scale factor for input images.
     * \param outputDepthScale The scale factor for output depth maps.
     * \param shadingBasedOptimization Flag indicating whether shading-based optimization is enabled.
     * \param semiGlobalMatching Flag indicating whether semi-global matching (SGM) is enabled.
     * \param surfaceSmoothingFactor The factor for surface smoothing.
     */
    Smvs(int inputImageScale,
                   int outputDepthScale,
                   bool shadingBasedOptimization,
                   bool semiGlobalMatching,
                   double surfaceSmoothingFactor);

    /*!
     * \brief Copy constructor for SMVS properties.
     * Constructs a new Smvs object by copying from another Smvs object.
     * \param smvs The Smvs object to copy from.
     */
    Smvs(const Smvs &smvs);

    /*!
     * \brief Move constructor for SMVS properties.
     * Constructs a new Smvs object by moving the resources of another Smvs object.
     * \param smvs The Smvs object to move from.
     */
    Smvs(Smvs &&smvs) noexcept;
    ~Smvs() override = default;

    /*!
     * \brief Copy assignment operator for SMVS properties.
     * Assigns values from another Smvs object to this object.
     * \param smvs The Smvs object to assign from.
     * \return Reference to the assigned Smvs object.
     */
    auto operator =(const Smvs& smvs) -> Smvs&;

    /*!
     * \brief Move assignment operator for SMVS properties.
     * Moves the resources from another Smvs object to this object.
     * \param smvs The Smvs object to move from.
     * \return Reference to the moved Smvs object.
     */
    auto operator =(Smvs&& smvs) noexcept -> Smvs&;


    /*!
     * \brief Get the scale factor for input images.
     * \return The scale factor for input images.
     */
    virtual auto inputImageScale() const -> int;

    /*!
     * \brief Get the scale factor for output depth maps.
     * \return The scale factor for output depth maps.
     */
    virtual auto outputDepthScale() const -> int;

    /*!
     * \brief Check if shading-based optimization is enabled.
     * \return True if shading-based optimization is enabled, false otherwise.
     */
    virtual auto shadingBasedOptimization() const -> bool;

    /*!
     * \brief Check if semi-global matching (SGM) is enabled.
     * \return True if semi-global matching (SGM) is enabled, false otherwise.
     */
    virtual auto semiGlobalMatching() const -> bool;

    /*!
     * \brief Get the surface smoothing factor.
     * \return The surface smoothing factor.
     */
    virtual auto surfaceSmoothingFactor() const -> double;

    /*!
     * \brief Set the scale factor for input images.
     * \param inputImageScale The scale factor for input images.
     */
    virtual void setInputImageScale(int inputImageScale);

    /*!
     * \brief Set the scale factor for output depth maps.
     * \param outputDepthScale The scale factor for output depth maps.
     */
    virtual void setOutputDepthScale(int outputDepthScale);

    /*!
     * \brief Enable or disable shading-based optimization.
     * \param shadingBasedOptimization True to enable shading-based optimization, false to disable it.
     */
    virtual void setShadingBasedOptimization(bool shadingBasedOptimization);

    /*!
     * \brief Enable or disable semi-global matching (SGM).
     * \param semiGlobalMatching True to enable semi-global matching (SGM), false to disable it.
     */
    virtual void setSemiGlobalMatching(bool semiGlobalMatching);

    /*!
     * \brief Set the surface smoothing factor.
     * \param surfaceSmoothingFactor The surface smoothing factor.
     */
    virtual void setSurfaceSmoothingFactor(double surfaceSmoothingFactor);

// Densification interface

public:

    void reset() override;
    auto name() const -> QString final;

private:

    int mInputImageScale;
    int mOutputDepthScale;
    bool mShadingBasedOptimization;
    bool mSemiGlobalMatching;
    double mSurfaceSmoothingFactor;
};


/*----------------------------------------------------------------*/

/*!
 * \brief Class for SMVS densification.
 *
 * This class represents a densifier using Shading-aware Multi-view Stereo (SMVS) reconstruction.
 * It inherits properties related to SMVS reconstruction from the Smvs class and functionality
 * for densification from the DensifierBase class.
 */
class SmvsDensifier
  : public Smvs,
    public DensifierBase
{

public:

    /*!
     * \brief Constructor for SMVS densifier.
     * Constructs an SmvsDensifier object with provided parameters.
     * \param[in] images Images used for densification.
     * \param[in] cameras Cameras associated with the images.
     * \param[in] poses Camera poses associated with the images.
     * \param[in] groundPoints Ground points.
     * \param[in] outputPath The output path for storing densification results.
     * \param[in] cuda Flag indicating whether to use CUDA for computation (default is false).
     * \param[in] autoSegmentation Flag indicating whether to enable auto-segmentation (default is false).
     */
    SmvsDensifier(const std::unordered_map<size_t, Image> &images,
                  const std::map<int, Camera> &cameras,
                  const std::unordered_map<size_t, CameraPose> &poses,
                  const std::vector<GroundPoint> &groundPoints,
                  const tl::Path &outputPath,
                  bool cuda = false,
                  bool autoSegmentation = false);
    ~SmvsDensifier() override;

    TL_DISABLE_COPY(SmvsDensifier)
    TL_DISABLE_MOVE(SmvsDensifier)

private:

    void clearPreviousModel() const;
    void writeMveFile();
    void densify();

// TaskBase

protected:

    void execute(tl::Progress *progressBar = nullptr) override;

private:

    std::unordered_map<size_t, size_t> mGraphosToMveIds;
    bool mAutoSegmentation;

};



} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_SMVS_H
