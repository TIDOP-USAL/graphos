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

#ifndef GRAPHOS_SCALE_MODEL_INTERFACE_H
#define GRAPHOS_SCALE_MODEL_INTERFACE_H

#include <array>

#include <QVector3D>

#include <tidop/math/algebra/matrix.h>

#include "graphos/core/mvp.h"

namespace graphos
{

class ScaleModel
  : public Model
{

  Q_OBJECT

public:

  ScaleModel(QObject *parent = nullptr) : Model(parent) {}
  ~ScaleModel() override = default;

  //virtual QVector3D offset() const = 0;
  virtual void setTransform(const tl::math::Matrix<double, 4, 4> &trasnform) = 0;

public slots:

  virtual void loadSettings() = 0;
  virtual void saveSettings() = 0;
};

} // namespace graphos


#endif // GRAPHOS_SCALE_MODEL_INTERFACE_H
