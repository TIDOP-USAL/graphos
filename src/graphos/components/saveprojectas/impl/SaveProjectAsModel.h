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

#ifndef GRAPHOS_SAVE_PROJECT_AS_MODEL_H
#define GRAPHOS_SAVE_PROJECT_AS_MODEL_H

#include "graphos/components/saveprojectas/SaveProjectAsModel.h"

namespace graphos
{

class Project;

class SaveProjectAsModelImp
  : public SaveProjectAsModel
{

    Q_OBJECT

public:

    SaveProjectAsModelImp(Project *project,
                          QObject *parent = nullptr);
    ~SaveProjectAsModelImp();

// SaveProjectAsModel interface

public slots:

    void save(const tl::Path &file) override;
    tl::Path graphosProjectsDirectory() const override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

protected:

    Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_SAVE_PROJECT_AS_MODEL_H
