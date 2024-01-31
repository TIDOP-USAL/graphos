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
                             const tl::Path &outputPath)
  : mImages(images),
    mCameras(cameras),
    mPoses(poses),
    mGroundPoints(groundPoints),
    mOutputPath(outputPath),
    mCuda(false),
    mFormat(UndistortImages::Format::tiff)
{

}

DensifierBase::~DensifierBase()
{
}

void DensifierBase::enableCuda(bool enable)
{
    mCuda = enable;
}

bool DensifierBase::isCudaEnabled() const
{
    return mCuda;
}

tl::Path DensifierBase::denseModel() const
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

void DensifierBase::undistort(const QString &dir)
{

    try {

        //TODO: Se están corrigiendo de distorsión todas las imágenes.
        //      Habría que ver si están orientadas
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

tl::Path DensifierBase::outputPath() const
{
    return mOutputPath;
}

const std::unordered_map<size_t, Image> &DensifierBase::images() const
{
    return mImages;
}

const std::map<int, Camera> &DensifierBase::cameras() const
{
    return mCameras;
}

const std::unordered_map<size_t, CameraPose> &DensifierBase::poses() const
{
    return mPoses;
}

const std::vector<GroundPoint> &DensifierBase::groundPoints() const
{
    return mGroundPoints;
}

void DensifierBase::setDenseModel(const tl::Path &denseModel)
{
    mDenseModel = denseModel;
}

void DensifierBase::autoSegmentation()
{
    CC_FILE_ERROR result = CC_FERR_NO_ERROR;
    ccHObject group;

    try {

        PlyFilter filter;
        FileIOFilter::LoadParameters parameters;
        parameters.alwaysDisplayLoadDialog = false;
        parameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
        parameters.parentWidget = nullptr;
        result = filter.loadFile(QString::fromStdString(mDenseModel.toString()),
                                 group,
                                 parameters);

        TL_ASSERT(result == CC_FERR_NO_ERROR, "Error loading point cloud");

        ccHObject::Container clouds;
        group.filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

        TL_ASSERT(clouds.size() == 1, "Error");

        ccHObject *model_3d = clouds.at(0);

        bool lockedVertices;
        ccGenericPointCloud *cloud = ccHObjectCaster::ToGenericPointCloud(model_3d, &lockedVertices);

        if (cloud && cloud->isA(CC_TYPES::POINT_CLOUD)) {

            ccPointCloud *pc = static_cast<ccPointCloud *>(cloud);

            //ccOctree::Shared theOctree = cloud->getOctree();
            //if (!theOctree)
            //{
            //	//ccProgressDialog pOctreeDlg(true, this);
            //	theOctree = cloud->computeOctree(&pOctreeDlg);
            //	if (!theOctree)
            //	{
            //		ccConsole::Error(tr("Couldn't compute octree for cloud '%1'!").arg(cloud->getName()));
            //		break;
            //	}
            //}

            //we create/activate CCs label scalar field
            int sfIdx = pc->getScalarFieldIndexByName(CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME);
            if (sfIdx < 0) {
                sfIdx = pc->addScalarField(CC_CONNECTED_COMPONENTS_DEFAULT_LABEL_NAME);
            }

            TL_ASSERT(sfIdx >= 0, "Couldn't allocate a new scalar field for computing CC labels! Try to free some memory ...");

            pc->setCurrentScalarField(sfIdx);

            //try to label all CCs
            int componentCount = CCCoreLib::AutoSegmentationTools::labelConnectedComponents(cloud,
                                                                                            static_cast<unsigned char>(8),
                                                                                            false,
                                                                                            nullptr);

            TL_ASSERT(componentCount > 0, "No component found!");

            pc->getCurrentInScalarField()->computeMinAndMax();
            CCCoreLib::ReferenceCloudContainer components;
            bool success = CCCoreLib::AutoSegmentationTools::extractConnectedComponents(pc, components);
            pc->deleteScalarField(sfIdx);
            sfIdx = -1;

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

                CCCoreLib::ReferenceCloud *compIndexes = components[index];

                ccPointCloud *compCloud = pc->partialClone(compIndexes);
                if (compCloud) {
                    compCloud->copyGlobalShiftAndScale(*pc);
                }

                FileIOFilter::SaveParameters save_parameters;
                save_parameters.alwaysDisplaySaveDialog = false;
                save_parameters.parentWidget = nullptr;
                //result = filter.saveToFile(group.getChild(0), QString::fromStdString(mDenseModel.toString()), save_parameters);
                result = filter.saveToFile(compCloud, QString::fromStdString(mDenseModel.toString()), save_parameters);

                TL_ASSERT(result == CC_FERR_NO_ERROR, "Save Ply error");

            }

        }

    } catch (...) {

        return;
    }

}


}
