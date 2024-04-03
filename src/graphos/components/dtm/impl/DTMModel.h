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

#ifndef GRAPHOS_DTM_MODEL_H
#define GRAPHOS_DTM_MODEL_H

#include "graphos/components/dtm/DTMModel.h"

namespace graphos
{

class Project;

class DtmModelImp
  : public DtmModel
{

public:

    DtmModelImp(Project *project, QObject *parent = nullptr);
    ~DtmModelImp() override = default;

// DtmModel interface

public:

    auto offset() const -> std::array<double, 3> override;
    auto projectPath() const -> tl::Path override;
    auto denseModel() const -> tl::Path override;
    auto crs() const -> QString override;
    auto gsd() const -> double override;
    auto dtmPath() const -> tl::Path override;
    auto dsmPath() const -> tl::Path override;

    void setGsd(double gsd) override;
    void setDtmPath(const tl::Path &dtmPath) override;
    void setDsmPath(const tl::Path &dsmPath) override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;

};


} // namespace graphos

#endif // GRAPHOS_DTM_MODEL_H
