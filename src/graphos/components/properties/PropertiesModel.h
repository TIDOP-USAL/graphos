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

#ifndef GRAPHOS_PROPERTIES_MODEL_INTERFACE_H
#define GRAPHOS_PROPERTIES_MODEL_INTERFACE_H

#include <unordered_map>

#include "graphos/core/mvp.h"

namespace graphos
{

/*!
 * \brief PropertiesModel interface
 */
class PropertiesModel
  : public Model
{

    Q_OBJECT

public:

    using Properties = std::unordered_map<QString, std::list<std::pair<QString, QString>>>;

public:

    PropertiesModel(QObject *parent = nullptr) : Model(parent) {}
    ~PropertiesModel() override = default;

    virtual auto sparseModel() const -> Properties = 0;
    virtual auto denseModel() const -> Properties = 0;
    virtual auto meshModel() const -> Properties = 0;
    virtual auto exif(size_t imageId) const -> Properties = 0;
    virtual auto parse(const QString& parser, const QString& file) const -> Properties = 0;
    virtual auto parse(const QStringList& parsers, const QStringList& files) const -> Properties = 0;
};



} // namespace graphos

#endif // GRAPHOS_PROPERTIES_MODEL_INTERFACE_H

