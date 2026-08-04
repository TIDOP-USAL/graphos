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

#include "graphos/graphos_global.h"

#include "tidop/core/base/Path.h"

#include "graphos/core/project/Project.h"

class QXmlStreamWriter;

namespace graphos
{

class ProjectWriter
{

public:

    void write(const tl::Path &file, const Project &project);

private:

    void writeInfo(QXmlStreamWriter &stream, const ProjectInfo &projectInfo);
    void writeCameras(QXmlStreamWriter &stream, const CameraRepository &cameraRepository);
    void writePriorCalibration(QXmlStreamWriter &stream, const Calibration *calibration);
    void writeCalibration(QXmlStreamWriter &stream, const Calibration *calibration);
    void writeVignetting(QXmlStreamWriter &stream, const Vignetting *vignetting);
    void writeImages(QXmlStreamWriter &stream, const ImageRepository &imageRepository);
    void writeCameraPosition(QXmlStreamWriter &stream, const CameraPose &cameraPosition);
    void writeImageMetadata(QXmlStreamWriter &stream, const Image::Metadata &metadata);
    void writeFeatures(QXmlStreamWriter &stream, const Project &project);
    void writeMatches(QXmlStreamWriter &stream, const Project &project);
    void writeOrientation(QXmlStreamWriter &stream, const Project &project);
    void writeDensification(QXmlStreamWriter &stream, const Project &project);
};

} // end namespace graphos
