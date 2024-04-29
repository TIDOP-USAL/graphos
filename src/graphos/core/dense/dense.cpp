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

#include "graphos/core/dense/dense.h"

#include "graphos/core/camera/Camera.h"
#include "graphos/core/image.h"

/* TidopLib */
#include <tidop/core/path.h>

/* CloudCompare*/
#include <FileIOFilter.h>
#include <PlyFilter.h>
#include <AutoSegmentationTools.h>
#include <ccHObjectCaster.h>
#include <ccGenericPointCloud.h>
#include <ccPointCloud.h>
#include <ccCommon.h>
#include <ReferenceCloud.h>

namespace graphos
{

DensifierBase::DensifierBase(const std::unordered_map<size_t, Image> &images,
                             const std::map<int, Camera> &cameras,
                             const std::unordered_map<size_t, CameraPose> &poses,
                             const std::vector<GroundPoint> &groundPoints,
                             tl::Path outputPath)
  : mImages(images),
    mCameras(cameras),
    mPoses(poses),
    mGroundPoints(groundPoints),
    mOutputPath(std::move(outputPath)),
    mCuda(false),
    mFormat(UndistortImages::Format::tiff)
{

}

DensifierBase::~DensifierBase() = default;

void DensifierBase::enableCuda(bool enable)
{
    mCuda = enable;
}

auto DensifierBase::isCudaEnabled() const -> bool
{
    return mCuda;
}

auto DensifierBase::denseModel() const -> tl::Path
{
    return mDenseModel;
}

auto DensifierBase::report() const -> DenseReport
{
    return mReport;
}

void DensifierBase::setUndistortImagesFormat(UndistortImages::Format format)
{
    mFormat = format;
}

auto DensifierBase::undistort(const QString &dir) const -> void
{

    try {

        UndistortImages undistort(mImages,
                                  mCameras,
                                  dir,
                                  mFormat,
                                  mCuda);
        undistort.run();

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }
}

auto DensifierBase::outputPath() const -> tl::Path
{
    return mOutputPath;
}

auto DensifierBase::images() const -> const std::unordered_map<size_t, Image>&
{
    return mImages;
}

auto DensifierBase::cameras() const -> const std::map<int, Camera>&
{
    return mCameras;
}

auto DensifierBase::poses() const -> const std::unordered_map<size_t, CameraPose>&
{
    return mPoses;
}

auto DensifierBase::groundPoints() const -> const std::vector<GroundPoint>&
{
    return mGroundPoints;
}

void DensifierBase::setDenseModel(const tl::Path &denseModel)
{
    mDenseModel = denseModel;
}

void DensifierBase::autoSegmentation() const
{
    try {

        ccHObject group;

        PlyFilter filter;
        FileIOFilter::LoadParameters parameters;
        parameters.alwaysDisplayLoadDialog = false;
        parameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
        parameters.parentWidget = nullptr;
        CC_FILE_ERROR result = CC_FERR_NO_ERROR;
        result = filter.loadFile(QString::fromStdString(mDenseModel.toString()),
                                 group,
                                 parameters);

        TL_ASSERT(result == CC_FERR_NO_ERROR, "Error loading point cloud");

        ccHObject::Container clouds;
        group.filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

        TL_ASSERT(clouds.size() == 1, "Error");

        ccHObject *model_3d = clouds.at(0);

        bool locked_vertices;
        ccGenericPointCloud *cloud = ccHObjectCaster::ToGenericPointCloud(model_3d, &locked_vertices);

        if (cloud && cloud->isA(CC_TYPES::POINT_CLOUD)) {

            auto pc = dynamic_cast<ccPointCloud *>(cloud);

            int scalar_field_index = pc->getScalarFieldIndexByName(CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME);
            if (scalar_field_index < 0) {
                scalar_field_index = pc->addScalarField(CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME);
            }

            TL_ASSERT(scalar_field_index >= 0, "Couldn't allocate a new scalar field for computing CC labels! Try to free some memory ...");

            pc->setCurrentScalarField(scalar_field_index);

            //try to label all CCs
            int component_count = CCCoreLib::AutoSegmentationTools::labelConnectedComponents(cloud, 8, false, nullptr);

            TL_ASSERT(component_count > 0, "No component found!");

            pc->getCurrentInScalarField()->computeMinAndMax();
            CCCoreLib::ReferenceCloudContainer components;
            bool success = CCCoreLib::AutoSegmentationTools::extractConnectedComponents(pc, components);
            pc->deleteScalarField(scalar_field_index);
            scalar_field_index = -1;

            TL_ASSERT(success, "An error occurred (failed to finish the extraction)");

            if (components.size() > 1) {

                size_t index = 0;
                size_t max = 0;
                for (unsigned i = 0; i < static_cast<unsigned>(components.size()); ++i) {
                    if (components[i]->size() > max) {
                        max = components[i]->size();
                        index = i;
                    }
                }

                //TODO: No quedarse sólo con la mayor nube de puntos. Comprobar el tamaño de las siguientes
                //std::map<size_t, size_t> map_size_index;
                //for (unsigned i = 0; i < static_cast<unsigned>(components.size()); ++i) {
                //    map_size_index[components[i]->size()] = i;
                //}

                //auto it = std::prev(map_size_index.end());
                //while (it != map_size_index.begin()){
                //    
                //}

                CCCoreLib::ReferenceCloud *component_indexes = components[index];

                ccPointCloud *component_cloud = pc->partialClone(component_indexes);
                if (component_cloud) {
                    component_cloud->copyGlobalShiftAndScale(*pc);
                }

                FileIOFilter::SaveParameters save_parameters;
                save_parameters.alwaysDisplaySaveDialog = false;
                save_parameters.parentWidget = nullptr;
                result = filter.saveToFile(component_cloud, QString::fromStdString(mDenseModel.toString()), save_parameters);

                TL_ASSERT(result == CC_FERR_NO_ERROR, "Save Ply error");

            }

        }

    } catch (...) {

        TL_THROW_EXCEPTION_WITH_NESTED("Exception caught in the automatic segmentation of the point cloud");
    }

}


}
