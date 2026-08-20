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

#pragma once

#include <unordered_map>
#include <stop_token>

#include <tidop/core/task/Task.h>
#include <tidop/core/base/Path.h>
#include <tidop/geometry/transform/Affine.h>
#include <tidop/graphic/entities/GPolygon.h>
//#include <tidop/vect/vectwriter.h>
//#include <tidop/graphic/entities/polygon.h>
//#include <tidop/math/geometry/affine.h>
//
#include "graphos/core/image/Image.h"
#include "graphos/core/camera/Camera.h"

namespace tl 
{
class Progress;
}

namespace graphos
{

//class Orthorectification;
//class ZBuffer;

class OrthoimageTask
	: public tl::Task
{

public:

	/*!
	 * \brief
	 */
	OrthoimageTask(const std::unordered_map<size_t, Image> &images,
	               const std::map<int, Camera> &cameras,
				   const tl::Path &dtm,
				   const tl::Path &orthoPath,
				   const tl::Path &graphOrthos,
				   const std::string &enuCrs,
				   const std::string &crs,
		           const tl::Affine<double, 2> &georeferenceOrthomosaic,
				   const tl::Path &footprint = tl::Path(),
				   double gsd = -1.,
				   const std::string &interpolation = "BILINEAR",
				   double crop = 1.,
				   bool bCuda = false);
	~OrthoimageTask();

// Heredado vía Task

private:

	void execute(tl::Progress *progressBar, std::stop_token stopToken) override;

	//auto orthoimageGraphProjected(const tl::Window<tl::Point<double>> &windowOrthoTerrain) const -> std::shared_ptr<tl::GPolygon>;
	auto footprintProjected(const tl::GPolygon &footprint) const -> std::unique_ptr<tl::GPolygon>;

private:

	const std::unordered_map<size_t, Image> &mImages;
	const std::map<int, Camera> &mCameras;
	tl::Path mDtm;
	tl::Path mOrthoPath;
	tl::Path mGraphOrthosPath;
    std::string mEnuCrs;
	std::string mCrs;
	tl::Affine<double, 2> mGeoreferenceOrthomosaic;
	tl::Path mFootprint;
	//std::unique_ptr<tl::VectorWriter> mFootprintWriter;
	//std::unique_ptr<tl::VectorWriter> mGraphOrthosWriter;
	double mGsd;
	std::string mInterpolation;
	double mCrop;
	bool bCuda;

};

} // namespace graphos
