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

#ifndef GRAPHOS_ORTHOPHOTO_ALGORITHM_H
#define GRAPHOS_ORTHOPHOTO_ALGORITHM_H

#include "graphos/graphos_global.h"

#include "graphos/core/utils.h"

#include <tidop/geospatial/photo.h>

#include <vector>
#include <map>
#include <string>

namespace graphos
{


class OrthophotoParameters
{ 

public:

  OrthophotoParameters();
  ~OrthophotoParameters();

  virtual double resolution() const;
  virtual void setResolution(double resolution);

  void clear();

private:

  double mResolution;

};





class OrthophotoAlgorithm
  : public OrthophotoParameters
{

public:

  OrthophotoAlgorithm();
  OrthophotoAlgorithm(double resolution,
                      const std::vector<tl::Photo> &photos,
                      const QString &orthoPath,
                      const QString &mdt,
                      const QString &epsg,
                      bool cuda = false);
  ~OrthophotoAlgorithm();

public:

  void run();

  void setPhotos(const std::vector<tl::Photo> &photos);
  void setOrthoPath(const QString &orthoPath);
  void setMdt(const QString &mdt);
  void setCrs(const QString &epsg);
  void setCuda(bool active);

private:

  std::vector<tl::Photo> mPhotos;
  QString mOrthoPath;
  QString mMdt;
  QString mEpsg;
  bool bCuda;
};


} // namespace graphos


#endif // GRAPHOS_ORTHOPHOTO_ALGORITHM_H
