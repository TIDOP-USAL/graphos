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

#ifndef GRAPHOS_ORTHOIMAGE_TASK_H
#define GRAPHOS_ORTHOIMAGE_TASK_H


#include <tidop/core/task.h>
#include <tidop/core/path.h>
#include <tidop/geospatial/crs.h>
#include <tidop/vect/vectwriter.h>
#include <tidop/geospatial/crstransf.h>

#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"


namespace tl 
{
class Progress;
}

namespace graphos
{

class Orthorectification;
class ZBuffer;

class OrthoimageTask
	: public tl::TaskBase
{

public:

	/*!
	 * \brief
	 */
	OrthoimageTask(const std::vector<Image> &images,
				   const std::map<int, Camera> &cameras,
				   const tl::Path &dtm,
				   const tl::Path &orthoPath,
				   const tl::Path &graphOrthos,
				   /// Reemplazar
                   const tl::EcefToEnu &ecefToEnu, 
                   const std::shared_ptr<tl::CrsTransform> &crsTransfom,
				   const tl::Crs &crs,
				   /// 
				   const tl::Path &footprint = tl::Path(),
				   double scale = -1,
				   double crop = 1,
				   bool bCuda = false);
	~OrthoimageTask();

// Heredado vía TaskBase

private:

	void execute(tl::Progress *progressBar = nullptr) override;

private:

	std::vector<Image> mImages;
	std::map<int, Camera> mCameras;
	tl::Path mDtm;
	tl::Path mOrthoPath;
	//tl::Point3<double> mOffset;
	tl::EcefToEnu mEcefToEnu;
    std::shared_ptr<tl::CrsTransform> mCrsTransfom;
	tl::Crs mCrs;
	std::unique_ptr<tl::VectorWriter> mFootprintWriter;
	std::unique_ptr<tl::VectorWriter> mGraphOrthosWriter;
	double mScale;
	double mCrop;
	bool bCuda;

};

} // namespace graphos

#endif // GRAPHOS_ORTHOIMAGE_TASK_H
