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

#ifndef GRAPHOS_CREATE_PROJECT_MODEL_H
#define GRAPHOS_CREATE_PROJECT_MODEL_H

#include "graphos/components/createproject/CreateProjectModel.h"

namespace graphos
{

class Project;


class CreateProjectModelImp
  : public CreateProjectModel
{

    Q_OBJECT

public:

    explicit CreateProjectModelImp(Project *project,
                                   QObject *parent = nullptr);
    ~CreateProjectModelImp() override;

// CreateProjectModel interface

public:

    tl::Path projectsDefaultPath() const override;

public slots:

    void setProjectName(const QString &name) override;
    void setProjectDescription(const QString &projectDescription) override;
    void setProjectFolder(const tl::Path &folder) override;
    void save() override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

private:

    Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_CREATE_PROJECT_MODEL_H

