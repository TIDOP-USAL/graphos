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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_ORTHOPHOTO_MODEL_INTERFACE_H
#define GRAPHOS_ORTHOPHOTO_MODEL_INTERFACE_H

#include <array>

#include "tidop/geospatial/photo.h"

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class OrthophotoParameters;

class OrthophotoModel
  : public Model
{

  Q_OBJECT

public:

  OrthophotoModel(QObject *parent = nullptr) : Model(parent) {}
  ~OrthophotoModel() override = default;
  
  virtual OrthophotoParameters *parameters() const = 0;
 
  virtual std::vector<tl::Photo> photos() const = 0;
  virtual QString orthoPath() const = 0;
  virtual QString dtmPath() const = 0;
  virtual QString epsCode() const = 0;
  virtual void clearProject() = 0;
  virtual bool useCuda() const = 0;

public slots:

  virtual void loadSettings() = 0;
  virtual void saveSettings() = 0;
};

} // namespace graphos


#endif // GRAPHOS_ORTHOPHOTO_MODEL_INTERFACE_H
