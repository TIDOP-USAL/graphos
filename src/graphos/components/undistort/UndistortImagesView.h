/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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


#ifndef GRAPHOS_UNDISTORTIMAGES_VIEW_INTERFACE_H
#define GRAPHOS_UNDISTORTIMAGES_VIEW_INTERFACE_H

#include <QFileDialog>

namespace graphos
{

class UndistortImagesView
  : public QFileDialog
{

  Q_OBJECT

public:

  UndistortImagesView(QWidget *parent) : QFileDialog(parent) {}
  ~UndistortImagesView() override = default;

  //virtual QString outputPath() const = 0;

public slots:

  virtual void setProjectDirectory(const QString &directory) = 0;

//signals:
//
//  void outputPathChanged(QString);

};

} // namespace graphos


#endif // GRAPHOS_UNDISTORTIMAGES_VIEW_INTERFACE_H