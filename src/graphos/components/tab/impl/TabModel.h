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

#ifndef GRAPHOS_TAB_MODEL_H
#define GRAPHOS_TAB_MODEL_H

#include "graphos/components/tab/TabModel.h"

namespace graphos
{

class Project;


class TabModelImp
  : public TabModel
{

  Q_OBJECT

public:

  explicit TabModelImp(Project *project,
                                 QObject *parent = nullptr);
  ~TabModelImp() override;

signals:

// TabModel interface

public:

  Image image(size_t imageId) const override;
  bool isImageOriented(size_t imageId) const override;
  CameraPose cameraPose(size_t imageId) const override;

public slots:

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

private:

  Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_TAB_MODEL_H

