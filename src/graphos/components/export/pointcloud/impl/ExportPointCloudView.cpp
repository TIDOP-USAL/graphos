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
 
#include "ExportPointCloudView.h"

namespace graphos
{

ExportPointCloudViewImp::ExportPointCloudViewImp(QWidget *parent)
  : ExportPointCloudView(parent)
{
    this->init();
}

void ExportPointCloudViewImp::setGraphosProjectsPath(const QString &directory)
{
    QFileDialog::setDirectory(directory);
}

void ExportPointCloudViewImp::init()
{
	QFileDialog::setWindowTitle(tr("Export Point Cloud"));
	QFileDialog::setNameFilter(tr("PLY (*.ply)"));
	//QFileDialog::setNameFilter(tr("LAS (*.las)"));
	//QFileDialog::setFileMode(QFileDialog::ExistingFile);
	QFileDialog::setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
}
 
} // namespace graphos