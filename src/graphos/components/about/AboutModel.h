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
 
#ifndef GRAPHOS_ABOUT_MODEL_INTERFACE_H
#define GRAPHOS_ABOUT_MODEL_INTERFACE_H

#include <tidop/core/defs.h>
#include "graphos/interfaces/mvp.h"

namespace tl
{
class Licence;
}

namespace graphos
{
  
class AboutModel
  : public Model
{

  Q_OBJECT

public:
  
  using const_iterator = std::list<tl::Licence>::const_iterator;

public:

  AboutModel(QObject *parent = nullptr) : Model(parent) {}
  ~AboutModel() override = default;

  virtual const tl::Licence &graphosLicence() const = 0;
  virtual QString readLicence(const QString &licence) = 0;

  virtual const_iterator begin() const = 0 ;
  virtual const_iterator end() const = 0 ;

};

} // namespace graphos

#endif // GRAPHOS_ABOUT_MODEL_INTERFACE_H