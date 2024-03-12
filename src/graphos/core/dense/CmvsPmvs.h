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

#ifndef GRAPHOS_CORE_DENSE_CMVS_PMVS_H
#define GRAPHOS_CORE_DENSE_CMVS_PMVS_H

#include "config_graphos.h"

#include "graphos/core/dense/dense.h"

#include <tidop/core/path.h>

#include <QString>

#include <unordered_map>

namespace graphos
{

/*!
 * \brief Properties for CMVS/PMVS densification.
 *
 * This class represents properties related to Clustering Views for Multi-view Stereo (CMVS) and
 * Patch-based Multi-view Stereo Software (PMVS). It inherits functionality for densification from
 * the Densification class.
 *
 * CMVS: Clustering Views for Multi-view Stereo
 * Website: https://www.di.ens.fr/cmvs/
 *
 * PMVS: Patch-based Multi-view Stereo Software (Version 2)
 * Website: https://www.di.ens.fr/pmvs/
 */
class CmvsPmvs
  : public Densification
{

public:

    /*!
     * \brief Default constructor for CMVS/PMVS properties.
     * Constructs a CmvsPmvs object with default values.
     */
    CmvsPmvs();

    /*!
     * \brief Parameterized constructor for CMVS/PMVS properties.
     * Constructs a CmvsPmvs object with provided parameters.
     * \param[in] useVisibilityInformation Flag indicating whether to use visibility information.
     * \param[in] imagesPerCluster The number of images per cluster.
     * \param[in] level The software internally builds an image pyramid, and this parameter specifies
     * the level in the image pyramid that is used for the computation. When level is 0, original (full)
     * resolution images are used. When level is 1, images are halved. When level is 2, images are 4 times smaller.
     * \param[in] cellSize Controls the density of reconstructions. The software tries to reconstruct at
     * least one patch in every csize x csize pixel square region in all the target images. Therefore,
     * increasing the value of csize leads to sparaser reconstructions.
     * \param[in] threshold A patch reconstruction is accepted as a success and kept, if its associcated
     * photometric consistency measure is above this threshold. Normalized cross correlation is used as a
     * photometric consistency measure, whose value ranges from -1 (bad) to 1 (good). The software repeats
     * three iterations of the reconstruction pipeline, and this threshold is relaxed (decreased) by 0.05
     * at the end of each iteration. For example, if you specify threshold=0.7, the values of the threshold
     * are 0.7, 0.65, and 0.6 for the three iterations of the pipeline, respectively.
     * \param[in] windowSize The software samples windowSize x windowSize pixel colors from each image to compute
     * photometric consistency score. For example, when windowSize=7, 7x7=49 pixel colors are sampled in each
     * image. Increasing the value leads to more stable reconstructions, but the program becomes slower.
     * \param[in] minimunImageNumber Each 3D point must be visible in the specified minimum number of images for being reconstructed.
     */
    CmvsPmvs(bool useVisibilityInformation,
                       int imagesPerCluster,
                       int level,
                       int cellSize,
                       double threshold,
                       int windowSize,
                       int minimunImageNumber);

    /*!
     * \brief Copy constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by copying from another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to copy from.
     */
    CmvsPmvs(const CmvsPmvs &cmvsPmvs);

    /*!
     * \brief Move constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by moving the resources of another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to move from.
     */
    CmvsPmvs(CmvsPmvs &&cmvsPmvs) noexcept;
    ~CmvsPmvs() override = default;

    /*!
     * \brief Move constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by moving the resources of another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to move from.
     */
    auto operator =(const CmvsPmvs& cmvsPmvs) -> CmvsPmvs&;

    /*!
     * \brief Move constructor for CMVS/PMVS properties.
     * Constructs a new CmvsPmvs object by moving the resources of another CmvsPmvs object.
     * \param[in] cmvsPmvs The CmvsPmvs object to move from.
     */
    auto operator =(CmvsPmvs&& cmvsPmvs) noexcept -> CmvsPmvs&;

    virtual auto useVisibilityInformation() const -> bool;
    virtual auto imagesPerCluster() const -> int;
    virtual auto level() const -> int;
    virtual auto cellSize() const -> int;
    virtual auto threshold() const -> double;
    virtual auto windowSize() const -> int;
    virtual auto minimunImageNumber() const -> int;

    virtual void setUseVisibilityInformation(bool useVisibilityInformation);
    virtual void setImagesPerCluster(int imagesPerCluster);
    virtual void setLevel(int level);
    virtual void setCellSize(int cellSize);
    virtual void setThreshold(double threshold);
    virtual void setWindowSize(int windowSize);
    virtual void setMinimunImageNumber(int minimunImageNumber);

// Densification interface

public:

    void reset() override;
    QString name() const override;

protected:

    bool mUseVisibilityInformation;
    int mImagesPerCluster;
    int mLevel;
    int mCellSize;
    double mThreshold;
    int mWindowSize;
    int mMinimunImageNumber;
    bool mImageOriginalDepth{false};
};


/*----------------------------------------------------------------*/

/*!
 * \brief Class for CMVS/PMVS densification.
 *
 * This class represents a densifier using Clustering Views for Multi-view Stereo (CMVS) and
 * Patch-based Multi-view Stereo Software (PMVS) reconstruction. It inherits properties related
 * to CMVS/PMVS densification from the CmvsPmvs class and functionality for densification
 * from the DensifierBase class.
 */
class CmvsPmvsDensifier
  : public CmvsPmvs,
    public DensifierBase
{

public:

    /*!
     * \brief Constructor for CMVS/PMVS densifier.
     * Constructs a CmvsPmvsDensifier object with provided parameters.
     * \param[in] images Images used for densification.
     * \param[in] cameras Cameras associated with the images.
     * \param[in] poses Camera poses associated with the images.
     * \param[in] groundPoints Ground points.
     * \param[in] outputPath The output path for storing densification results.
     * \param[in] database The path to the database.
     * \param[in] cuda Flag indicating whether to use CUDA for computation (default is false).
     * \param[in] autoSegmentation Flag indicating whether to enable auto-segmentation (default is false).
     */
    CmvsPmvsDensifier(const std::unordered_map<size_t, Image> &images,
                      const std::map<int, Camera> &cameras,
                      const std::unordered_map<size_t, CameraPose> &poses,
                      const std::vector<GroundPoint> &groundPoints,
                      const tl::Path &outputPath,
                      tl::Path database,
                      bool cuda = false,
                      bool autoSegmentation = false);
    ~CmvsPmvsDensifier() override;

    TL_DISABLE_COPY(CmvsPmvsDensifier)
    TL_DISABLE_MOVE(CmvsPmvsDensifier)

private:

    void clearPreviousModel() const;
    void writeBundleFile();
    void writeVisibility();
    void writeOptions() const;
    void densify();

// TaskBase

protected:

    /*!
     * \brief Executes the densification task.
     * Overrides the execute method from the TaskBase class.
     * \param[in] progressBar Pointer to a progress bar for tracking progress (default is nullptr).
     */
    void execute(tl::Progress *progressBar = nullptr) override;

private:

    std::unordered_map<size_t, size_t> mGraphosToBundlerIds;
    tl::Path mDatabase;
    bool mAutoSegmentation;
    //DenseReport mReport;
};


} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_CMVS_PMVS_H
