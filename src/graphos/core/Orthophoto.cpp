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

#include "Orthophoto.h"

#include <tidop/core/exception.h>


namespace graphos
{


/// Definir valores por defecto
/*constexpr auto DefaultOrthophoto[propiety] = 1;*/



/* OrthophotoParameters */

OrthophotoParameters::OrthophotoParameters()
  : 
    mResolution(0.01)
{
}

OrthophotoParameters::~OrthophotoParameters()
{
}

double OrthophotoParameters::resolution() const 
{
  return mResolution;
}


void OrthophotoParameters::setResolution(double resolution) 
{
  mResolution = resolution;
}



void OrthophotoParameters::clear()
{
  /*m[propiety] = DefaultOrthophoto[propiety];*/
}

} // namespace graphos
