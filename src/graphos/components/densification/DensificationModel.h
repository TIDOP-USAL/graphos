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

#ifndef GRAPHOS_DENSE_MODEL_INTERFACE_H
#define GRAPHOS_DENSE_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class Densification;

class DensificationModel
  : public Model
{

  Q_OBJECT

public:

  DensificationModel(QObject *parent = nullptr) : Model(parent) {}
  ~DensificationModel() override = default;

  virtual std::shared_ptr<Densification> densification() const = 0;
  virtual QString projectFolder() const = 0;
  virtual QString reconstructionPath() const = 0;
  virtual bool useCuda() const = 0;
  virtual bool existDenseModel() const = 0;

public slots:

  virtual void setDensification(const std::shared_ptr<Densification> &densification) = 0;
  virtual void setDenseModel(const QString &denseModel) = 0;
};

} // End namespace graphos

#endif // GRAPHOS_DENSE_MODEL_INTERFACE_H
