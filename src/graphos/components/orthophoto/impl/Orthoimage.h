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

#ifndef GRAPHOS_CORE_ORTHO_ORTHOIMAGE_H
#define GRAPHOS_CORE_ORTHO_ORTHOIMAGE_H

#include <tidop/core/path.h>
#include <tidop/core/task.h>
#include <tidop/img/imgreader.h>
#include <tidop/img/imgwriter.h>
#include <tidop/math/geometry/affine.h>
#include <tidop/geometry/rect.h>

#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"


namespace graphos
{

class Orthorectification;
class ZBuffer;

cv::Mat createBlackPixelMask(const cv::Mat &image, double areaThreshold = 4, bool upper = false);

/*!
 * \brief Orthoimage
 */
class Orthoimage
{

public:

	Orthoimage(const tl::Path &image,
			   Orthorectification *orthorectification,
			   const std::string &enuCrs,
			   const std::string &crs,
			   const tl::Rect<int> &rectOrtho,
			   const tl::Affine<double, 2> &georeference,
			   const std::string &interpolation = "BILINEAR",
			   bool cuda = false);

	~Orthoimage();

	void run(const tl::Path &ortho,
			 const cv::Mat &visibilityMap = cv::Mat());

private:

	std::unique_ptr<tl::ImageReader> mImageReader;
	Orthorectification *mOrthorectification;
	std::string mEnuCrs;
	std::string mCrs;
	tl::Rect<int> mRectOrtho;
	tl::Affine<double, 2> mGeoreference;
	std::unique_ptr<tl::ImageWriter> mOrthophotoWriter;
	tl::Window<tl::Point<double>> mWindowOrthoTerrain;
	std::string mInterpolation;
	bool bCuda;
};



} // End namespace graphos


#endif // GRAPHOS_CORE_ORTHO_ORTHOIMAGE_H
