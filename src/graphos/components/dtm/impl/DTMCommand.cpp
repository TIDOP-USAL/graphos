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


#include "DTMCommand.h"

#include "graphos/core/utils.h"
#include "graphos/core/project.h"
#include "graphos/components/dtm/impl/DTMTask.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/log.h>

#include <QFileInfo>

using namespace tl;

namespace graphos
{


DTMCommand::DTMCommand()
  : Command("dem", "Create DSM and/or DTM"),
    mProject(nullptr)
{
    this->addArgument<Path>("prj", 'p', "Project file");
    this->addArgument<double>("gsd", 'g', "Ground sample distance", 0.1);
    this->addArgument<bool>("dsm", "Create a Digital Surface Model", true);
    this->addArgument<bool>("dtm", "Create a Digital Terrain Model", false);

    this->addExample("dem -p 253/253.xml --gsd 0.1");

    this->setVersion(std::to_string(GRAPHOS_VERSION_MAJOR).append(".").append(std::to_string(GRAPHOS_VERSION_MINOR)));
}

DTMCommand::~DTMCommand()
{
    if (mProject) {
        delete mProject;
        mProject = nullptr;
    }
}

auto DTMCommand::offset() const -> std::array<double, 3>
{
    std::array<double, 3> offset{};
    offset.fill(0.);

    try {

        QFile file(QString::fromStdWString(mProject->offset().toWString()));
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

bool DTMCommand::run()
{
    bool r = false;

    tl::Log &log = tl::Log::instance();

    try {

        tl::Path project_path = this->value<Path>("prj");
        auto gsd =  this->value<double>("gsd");
        auto dsm =  this->value<bool>("dsm");
        auto dtm =  this->value<bool>("dtm");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        mProject = new ProjectImp;
        mProject->load(project_path);

        tl::Path dtm_path(mProject->projectFolder());
        dtm_path.append("dtm");
		
        tl::Path ground_points_path(mProject->reconstructionPath());
        ground_points_path.append("ground_points.bin");

        DtmTask dtm_task(mProject->denseModel(), offset(), dtm_path, gsd, mProject->crs(), dsm, dtm);
        dtm_task.run();

        tl::Path dsm_file = dtm_path;
        dsm_file.append("dsm.tif");
        if (dsm && dsm_file.exists()) {
            mProject->dtm().dsmPath = dsm_file;
        }
        
        tl::Path dtm_file = dsm_file;
        dtm_file.replaceBaseName("dtm");
        if (dtm && dtm_file.exists()) {
            mProject->dtm().dtmPath = dtm_file;
        }

        mProject->dtm().gsd = gsd;
        mProject->save(project_path);

    } catch (const std::exception &e) {

        printException(e);

        r = true;
    }

    log.close();

    return r;
}

} // namespace graphos