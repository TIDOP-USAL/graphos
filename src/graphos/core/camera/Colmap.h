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

#ifndef GRAPHOS_CORE_COLMAP_CAMERA_H
#define GRAPHOS_CORE_COLMAP_CAMERA_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>


namespace colmap
{
class Reconstruction;
}

namespace graphos
{

class Calibration;
class Camera;

///TODO: borrar
class ReadCalibration
{

public:

    ReadCalibration();
    ~ReadCalibration();

    void open(const QString &path);
    auto calibration(int cameraId) const -> std::shared_ptr<Calibration>;

protected:

    colmap::Reconstruction *mReconstruction;

};


/*!
 * \brief Convert camera type to Colmap camera type
 * \param[in] camera GRAPHOS camera type
 * \return Colmap camera type
 */
auto cameraToColmapType(const Camera &camera) -> QString;

} // namespace graphos

#endif // GRAPHOS_CORE_COLMAP_CAMERA_H
