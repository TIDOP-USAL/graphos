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

#ifndef GRAPHOS_OPEN_PROJECT_VIEW_H
#define GRAPHOS_OPEN_PROJECT_VIEW_H

#include "graphos/components/openproject/OpenProjectView.h"

namespace graphos
{

class OpenProjectViewImp
  : public OpenProjectView
{

    Q_OBJECT

public:

    explicit OpenProjectViewImp(QWidget *parent = nullptr);
    ~OpenProjectViewImp() override = default;

private:

    void init();

private:

// OpenProjectView interface

public:

    void setGraphosProjectsPath(const QString &directory) override;

};

} // namespace graphos

#endif // GRAPHOS_OPEN_PROJECT_VIEW_H
