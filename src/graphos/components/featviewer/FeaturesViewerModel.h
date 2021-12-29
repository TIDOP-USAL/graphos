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

#ifndef GRAPHOS_FEATURES_VIEWER_MODEL_INTERFACE_H
#define GRAPHOS_FEATURES_VIEWER_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class FeaturesViewerModel
  : public Model
{

  Q_OBJECT

public:

  FeaturesViewerModel(QObject *parent = nullptr) : Model(parent) {}
  ~FeaturesViewerModel() = default;

  virtual std::vector<QString> images() const = 0;
  virtual std::vector<QPointF> loadKeypoints(const QString &image) = 0;

  virtual QString viewerBGColor() const = 0;
  virtual int viewerMarkerType() const = 0;
  virtual int viewerMarkerSize() const = 0;
  virtual int viewerMarkerWidth() const = 0;
  virtual QString viewerMarkerColor() const = 0;
  virtual int viewerSelectMarkerWidth() const = 0;
  virtual QString viewerSelectMarkerColor() const = 0;


};

} // namespace graphos

#endif // GRAPHOS_FEATURES_VIEWER_MODEL_INTERFACE_H
