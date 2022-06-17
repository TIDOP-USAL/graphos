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
 
#ifndef GRAPHOS_LOADFROMVIDEO_MODEL_H
#define GRAPHOS_LOADFROMVIDEO_MODEL_H

#include "graphos/components/loadfromvideo/LoadFromVideoModel.h"
#include "graphos/core/project.h"

class QSettings;


namespace graphos
{

class LoadFromVideoModelImp
  : public LoadFromVideoModel
{

  Q_OBJECT

public:

  LoadFromVideoModelImp(Project *project, QObject *parent = nullptr);
  ~LoadFromVideoModelImp() override;

// LoadFromVideoModel interface

public:

  LoadFromVideoParameters *parameters() const override;

public slots:

  void loadSettings() override;
  void saveSettings() override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QSettings *mSettings;
  bool mReadSettings;
  LoadFromVideoParameters *mParameters;
};

} // namespace graphos

#endif // GRAPHOS_LOADFROMVIDEO_MODEL_H
