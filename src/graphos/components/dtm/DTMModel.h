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

#ifndef GRAPHOS_DTM_MODEL_INTERFACE_H
#define GRAPHOS_DTM_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"

#include <tidop/core/path.h>

namespace graphos
{

class Dtm;

class DtmModel
  : public Model
{

public:

    DtmModel(QObject *parent = nullptr) : Model(parent) {}
    ~DtmModel() override = default;

    virtual std::array<double, 3> offset() const = 0;
    virtual tl::Path projectPath() const = 0;
    virtual tl::Path denseModel() const = 0;
    virtual QString crs() const = 0;

    virtual std::shared_ptr<Dtm> dtmMethod() const = 0;
    virtual void setDtmMethod(const std::shared_ptr<Dtm> &dtm) = 0;
    virtual tl::Path dtmPath() const = 0;
    virtual void setDtmPath(const tl::Path &dtmPath) = 0;
};

} // namespace graphos

#endif // GRAPHOS_DTM_MODEL_INTERFACE_H
