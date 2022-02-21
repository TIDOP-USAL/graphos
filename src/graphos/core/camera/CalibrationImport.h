/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#ifndef GRAPHOS_CAMERA_CALIBRATION_IO_H
#define GRAPHOS_CAMERA_CALIBRATION_IO_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QString>

#include <tidop/geospatial/camera.h>

namespace graphos
{


class GRAPHOS_EXPORT CalibrationIO
{

public:

  CalibrationIO();
  virtual ~CalibrationIO() = default;

  virtual bool write(std::shared_ptr<tl::Calibration> &calibration, 
                     const QString &file) = 0;
  virtual std::shared_ptr<tl::Calibration> read(const QString &file) = 0;

};



/*----------------------------------------------------------------*/



class GRAPHOS_EXPORT CalibrationIOFactory
{

private:

  CalibrationIOFactory() {}

public:

  static std::unique_ptr<CalibrationIO> create(const QString &format);

};

} // namespace graphos


#endif // GRAPHOS_CAMERA_CALIBRATION_IO_H