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

#ifndef GRAPHOS_FEATURE_MATCHING_MODEL_INTERFACE_H
#define GRAPHOS_FEATURE_MATCHING_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{
	
class FeatureMatching;

class FeatureMatchingModel
  : public Model
{

  Q_OBJECT

public:

  FeatureMatchingModel(QObject *parent = nullptr) : Model(parent) {}
  ~FeatureMatchingModel() override = default;

  virtual std::shared_ptr<FeatureMatching> featureMatching() const = 0;
  virtual void setFeatureMatching(const std::shared_ptr<FeatureMatching> &featureMatching) = 0;
  virtual QString database() const = 0;
  virtual bool useCuda() const = 0;
  virtual bool spatialMatching() const = 0;
  virtual void writeMatchPairs() = 0;
  virtual bool existsMatches() const = 0;
  virtual void clearProject() = 0;
};

} // End namespace graphos

#endif // GRAPHOS_FEATURE_MATCHING_MODEL_INTERFACE_H