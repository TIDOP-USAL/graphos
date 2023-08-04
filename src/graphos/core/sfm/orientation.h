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

#ifndef GRAPHOS_CORE_ORIENTATION_H
#define GRAPHOS_CORE_ORIENTATION_H

#include "graphos/graphos_global.h"

#include <tidop/core/flags.h>

#include <QString>

namespace graphos
{

class RelativeOrientation
{

public:

    enum class Method
    {
        colmap
    };

public:

    RelativeOrientation() {}
    virtual ~RelativeOrientation() = default;

    virtual void reset() = 0;
    virtual QString name() const = 0;
    Method method() const { return mOrientationMethod.flags(); }

protected:

    tl::EnumFlags<Method> mOrientationMethod;

};
ALLOW_BITWISE_FLAG_OPERATIONS(RelativeOrientation::Method)



/*----------------------------------------------------------------*/



//class RelativeOrientationAlgorithm
//{
//
//public:
//
//  RelativeOrientationAlgorithm() {}
//  virtual ~RelativeOrientationAlgorithm() = default;
//
//  virtual void run() = 0;
//};


/*----------------------------------------------------------------*/

class AbsoluteOrientation
{

public:

    enum class Method
    {
        colmap
    };

public:

    AbsoluteOrientation() {}
    virtual ~AbsoluteOrientation() = default;

    virtual void reset() = 0;
    virtual QString name() const = 0;
    Method method() const { return mOrientationMethod.flags(); }

protected:

    tl::EnumFlags<Method> mOrientationMethod;
};
ALLOW_BITWISE_FLAG_OPERATIONS(AbsoluteOrientation::Method)


/*----------------------------------------------------------------*/


//class AbsoluteOrientationAlgorithm
//{
//
//public:
//
//  AbsoluteOrientationAlgorithm() {}
//  virtual ~AbsoluteOrientationAlgorithm() = default;
//
//  virtual void run() = 0;
//};

} // namespace graphos

#endif // GRAPHOS_CORE_ORIENTATION_H
