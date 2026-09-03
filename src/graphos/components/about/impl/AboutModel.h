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
 
#pragma once

#include "graphos/components/about/AboutModel.h"

namespace tl
{
class AppLicense;
}

namespace graphos
{

class AboutModelImp
  : public AboutModel
{

    Q_OBJECT

public:

    AboutModelImp(QObject *parent = nullptr);
    ~AboutModelImp() override;

// AboutModel interface

public:

    auto license() const -> const tl::License & override;
    auto readLicense(const QString &license) -> QString override;
    auto begin() const -> const_iterator override;
    auto end() const -> const_iterator override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

private:

    tl::AppLicense *mAppLicense;
};

} // namespace graphos