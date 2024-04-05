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

#ifndef GRAPHOS_PROPERTIES_PRESENTER_INTERFACE_H
#define GRAPHOS_PROPERTIES_PRESENTER_INTERFACE_H

#include "graphos/core/mvp.h"

namespace graphos
{

/*!
 * \brief PropertiesPresenter interface
 */
class PropertiesPresenter
  : public Presenter
{

    Q_OBJECT

public:

    PropertiesPresenter() {}
    ~PropertiesPresenter() override = default;

public slots:

    virtual void selectSparseModel() = 0;
    virtual void selectDenseModel() = 0;
    virtual void selectMeshModel() = 0;
    virtual void setImageActive(size_t imageId) = 0;
    virtual void parseDocument(const QString &parser, const QString &file) = 0;
    virtual void parseDocuments(const QStringList &parsers, const QStringList &files) = 0;
};

} // namespace graphos


#endif // GRAPHOS_PROPERTIES_PRESENTER_INTERFACE_H
