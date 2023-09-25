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

#ifndef GRAPHOS_PROPERTIES_PRESENTER_H
#define GRAPHOS_PROPERTIES_PRESENTER_H

#include "graphos/components/properties/PropertiesPresenter.h"

namespace graphos
{

class AppStatus;
class PropertiesView;
class PropertiesModel;

class PropertiesPresenterImp
  : public PropertiesPresenter
{
    Q_OBJECT

public:

    PropertiesPresenterImp(PropertiesView *view,
                           PropertiesModel *model,
                           AppStatus *status);
    ~PropertiesPresenterImp() override = default;

// PropertiesPresenter interface

public slots:

    void setImageActive(size_t imageId) override;
    void parseDocument(const QString &parser, const QString &file) override;

// Presenter interface

public slots:

    void open() override;

private:

    void init() override;
    void initSignalAndSlots() override;

private:

    PropertiesView *mView;
    PropertiesModel *mModel;
    AppStatus *mAppStatus;

};

} // namespace graphos


#endif // GRAPHOS_PROPERTIES_PRESENTER_H
