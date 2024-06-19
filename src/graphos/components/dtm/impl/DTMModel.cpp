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

#include "DTMModel.h"

#include "graphos/core/project.h"

#include <QFileInfo>
#include <QFile>
#include <QTextStream>

namespace graphos
{

DtmModelImp::DtmModelImp(Project *project, QObject *parent)
  : DtmModel(parent),
    mProject(project)
{
    DtmModelImp::init();
}

auto DtmModelImp::projectPath() const -> tl::Path
{
    return mProject->projectFolder();
}

auto DtmModelImp::denseModel() const -> tl::Path
{
    return mProject->denseModel();
}

auto DtmModelImp::crs() const -> QString
{
    return mProject->crs();
}

auto DtmModelImp::gsd() const -> double
{
    return mProject->dtm().gsd;
}

void DtmModelImp::setGsd(double gsd)
{
    mProject->dtm().gsd = gsd;
}

auto DtmModelImp::dtmPath() const -> tl::Path
{
    return mProject->dtm().dtmPath;
}

auto DtmModelImp::dsmPath() const -> tl::Path
{
    return mProject->dtm().dsmPath;
}

void DtmModelImp::setDtmPath(const tl::Path &dtmPath)
{
    mProject->dtm().dtmPath = dtmPath;
}

void DtmModelImp::setDsmPath(const tl::Path &dsmPath)
{
    mProject->dtm().dsmPath = dsmPath;
}

auto DtmModelImp::offset() const -> std::array<double, 3>
{
    std::array<double, 3> offset{};
    offset.fill(0.);

    try {

        tl::Path path = mProject->offset();
        QFile file(QString::fromStdWString(path.toWString()));
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);
            QString line = stream.readLine();
            QStringList reg = line.split(" ");
            offset[0] = reg[0].toDouble();
            offset[1] = reg[1].toDouble();
            offset[2] = reg[2].toDouble();
            file.close();
        }

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("");
    }

    return offset;
}

void DtmModelImp::init()
{
}

void DtmModelImp::clear()
{
}

} // namespace graphos


