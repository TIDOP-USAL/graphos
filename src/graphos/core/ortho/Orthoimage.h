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
#include <tidop/vect/vectwriter.h>
#include <tidop/geometry/transform/affine.h>
#include <tidop/geometry/rect.h>
#include <tidop/geospatial/crs.h>

#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"


namespace graphos
{

class Orthorectification;
class ZBuffer;

/*!
 * \brief Orthoimage
 */
class Orthoimage
{

public:

	Orthoimage(const tl::Path &image,
			   Orthorectification *orthorectification,
			   const tl::Crs &crs,
			   const tl::Rect<int> &rectOrtho,
			   const tl::Affine<tl::Point<double>> &georeference,
			   bool cuda = false);
	~Orthoimage();

	void run(const tl::Path &ortho,
			 const cv::Mat &visibilityMap = cv::Mat());

private:

	std::unique_ptr<tl::ImageReader> mImageReader;
	Orthorectification *mOrthorectification;
	tl::Crs mCrs;
	tl::Rect<int> mRectOrtho;
	tl::Affine<tl::Point<double>> mGeoreference;
	std::unique_ptr<tl::ImageWriter> mOrthophotoWriter;
	tl::Window<tl::Point<double>> mWindowOrthoTerrain;
	bool bCuda;
};



/*!
 * \brief OrthoimageProcess
 */
class OrthoimageProcess
	: public tl::TaskBase
{

public:

	/*!
	 * \brief
	 */
	OrthoimageProcess(const std::vector<Image> &images,
					  const std::map<int, Camera> &cameras,
					  const tl::Path &dtm,
					  const tl::Path &orthoPath,
					  const tl::Path &graphOrthos,
					  const tl::Crs &crs,
					  const tl::Path &footprint = tl::Path(),
					  double scale = -1,
					  double crop = 1,
					  bool bCuda = false);
	~OrthoimageProcess();

private:

	cv::Mat visibilityMap(const Orthorectification &orthorectification,
						  const ZBuffer &zBuffer) const;

// Heredado vía TaskBase

private:

	void execute(tl::Progress *progressBar = nullptr) override;

private:

	std::vector<Image> mImages;
	std::map<int, Camera> mCameras;
	tl::Path mDtm;
	tl::Path mOrthoPath;
	tl::Crs mCrs;
	std::unique_ptr<tl::VectorWriter> mFootprintWriter;
	std::unique_ptr<tl::VectorWriter> mGraphOrthosWriter;
	double mScale;
	double mCrop;
	bool bCuda;
};


} // End namespace graphos


#endif // GRAPHOS_CORE_ORTHO_ORTHOIMAGE_H
