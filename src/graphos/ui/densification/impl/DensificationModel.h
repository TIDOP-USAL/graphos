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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_DENSE_MODEL_H
#define GRAPHOS_DENSE_MODEL_H

#include <QObject>

#include "graphos/ui/densification/DensificationModel.h"

namespace graphos
{

class Project;

class DensificationModelImp
  : public DensificationModel
{
  Q_OBJECT

public:

  DensificationModelImp(Project *project,
                        QObject *parent = nullptr);
  ~DensificationModelImp() override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

// DensificationModel interface

public:

  std::shared_ptr<Densification> densification() const override;
  QString projectFolder() const override;
  QString reconstructionPath() const override;
  bool useCuda() const override;
  bool existDenseModel() const override;

public slots:

  void setDensification(const std::shared_ptr<Densification> &densification) override;
  void setDenseModel(const QString &denseModel) override;

protected:

  Project *mProject;

};

} // End namespace graphos

#endif // GRAPHOS_DENSE_MODEL_H
