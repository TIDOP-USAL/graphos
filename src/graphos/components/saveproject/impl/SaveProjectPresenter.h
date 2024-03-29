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

#ifndef GRAPHOS_SAVE_PROJECT_PRESENTER_H
#define GRAPHOS_SAVE_PROJECT_PRESENTER_H

#include "graphos/components/saveproject/SaveProjectPresenter.h"

namespace graphos
{

class SaveProjectView;
class SaveProjectModel;
class AppStatus;

class SaveProjectPresenterImp
  : public SaveProjectPresenter
{

    Q_OBJECT

public:

    SaveProjectPresenterImp(SaveProjectView *view,
                            SaveProjectModel *model,
                            AppStatus *status);
    ~SaveProjectPresenterImp() override;

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

private:

    SaveProjectView *mView;
    SaveProjectModel *mModel;
    AppStatus *mAppStatus;
};

} // namespace graphos

#endif // GRAPHOS_SAVE_PROJECT_PRESENTER_H
