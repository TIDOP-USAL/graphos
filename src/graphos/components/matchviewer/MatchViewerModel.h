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

#ifndef GRAPHOS_MATCH_VIEWER_MODEL_INTERFACE_H
#define GRAPHOS_MATCH_VIEWER_MODEL_INTERFACE_H

#include <memory>
#include <unordered_map>

#include "graphos/core/mvp.h"
#include "graphos/core/image.h"

namespace graphos
{

class MatchViewerModel
  : public Model
{

  Q_OBJECT

public:

  MatchViewerModel(QObject *parent = nullptr) : Model(parent) {}
  virtual ~MatchViewerModel() = default;

  virtual QString backgroundColor() const = 0;
  virtual int markerType() const = 0;
  virtual int markerSize() const = 0;
  virtual int markerWidth() const = 0;
  virtual QString markerColor() const = 0;
  virtual int viewerSelectMarkerWidth() const = 0;
  virtual QString selectedMarkerColor() const = 0;
  virtual QString lineColor() const = 0;
  virtual int lineWidth() const = 0;

  virtual const std::unordered_map<size_t, Image> &images() const = 0;
  virtual Image image(size_t imageId) const = 0;
  virtual std::vector<size_t> imagePairs(size_t imageId) const = 0;
  virtual std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> loadMatches(size_t imageId1,
                                                                                        size_t imageId2) const = 0;

public slots:

};

} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_MODEL_INTERFACE_H
