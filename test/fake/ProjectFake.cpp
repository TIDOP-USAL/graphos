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


#include "ProjectFake.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

namespace graphos
{

ProjectFake::ProjectFake()
{
  mProjectFileText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                     "<Graphos version=\"1.0\">"
                     "    <General>"
                     "        <Name>prj001</Name>"
                     "        <Path>C:/Users/User01/Documents/graphos/Projects/prj001</Path>"
                     "        <Description>Project 1</Description>"
                     "    </General>"
                     "</Graphos>";
}

void ProjectFake::load(const tl::Path &file)
{
    QXmlStreamReader stream;
    stream.addData(mProjectFileText);

    this->read(stream);
}

void ProjectFake::save(const tl::Path& file)
{
    mProjectPath = file;
}

} // end namespace graphos
