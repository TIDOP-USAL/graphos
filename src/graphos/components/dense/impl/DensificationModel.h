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

#ifndef GRAPHOS_DENSE_MODEL_H
#define GRAPHOS_DENSE_MODEL_H

#include <QObject>

#include "graphos/components/dense/DensificationModel.h"

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
  tl::Path projectFolder() const override;
  tl::Path reconstructionPath() const override;
  tl::Path database() const override;
  bool useCuda() const override;
  bool existDenseModel() const override;
  const std::unordered_map<size_t, Image> &images() const override;
  const std::map<int, Camera> &cameras() const override;
  const std::unordered_map<size_t, CameraPose> &poses() const override;
  std::vector<GroundPoint> groundPoints() const override;

public slots:

  void setDensification(const std::shared_ptr<Densification> &densification) override;
  void setDenseModel(const tl::Path &denseModel) override;

protected:

  Project *mProject;

};

} // End namespace graphos

#endif // GRAPHOS_DENSE_MODEL_H
