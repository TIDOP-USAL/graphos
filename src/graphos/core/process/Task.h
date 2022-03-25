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


#ifndef GRAPHOS_CORE_TASK_H
#define GRAPHOS_CORE_TASK_H


#include <tidop/core/process.h>
#include <tidop/core/progress.h>

/// TODO: la clase Command extendera las clases tl::Command y la clase Task
///       La clase Task a su vez extendera la clase tl::ProcessBase
///       La clase PresenterProcess tendra una instancia de Task que se le pasará por
///       inyección de dependencias. Se construira como un objeto Command aunque al
///       Presentador se le pasara como Task
///       


namespace graphos
{

class Task
  : public tl::ProcessBase
{

public:

  Task() : tl::ProcessBase(){}
  virtual ~Task() = default;

};


} // namespace graphos


#endif // GRAPHOS_CORE_TASK_H
