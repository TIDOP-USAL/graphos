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

#ifndef GRAPHOS_PROPERTIES_MODEL_H
#define GRAPHOS_PROPERTIES_MODEL_H

#include "graphos/components/properties/PropertiesModel.h"

namespace graphos
{

class Project;


class PropertiesModelImp
  : public PropertiesModel
{

    Q_OBJECT

public:

    explicit PropertiesModelImp(Project *project,
                                QObject *parent = nullptr);
    ~PropertiesModelImp() override;

signals:

// PropertiesModel interface

public:

    auto sparseModel() const -> Properties override;
    auto denseModel() const -> Properties override;
    auto meshModel() const -> Properties override;
    auto exif(size_t imageId) const -> Properties override;
    auto parse(const QString& parser, const QString& file) const -> Properties override;
    auto parse(const QStringList& parsers, const QStringList& files) const -> Properties override;

// Model interface

private:

    void init() override;

public slots:

    void clear() override;

private:

    Project *mProject;

};

} // namespace graphos

#endif // GRAPHOS_PROPERTIES_MODEL_H

