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

#ifndef GRAPHOS_CORE_ORTHO_FOOTPRINT_H
#define GRAPHOS_CORE_ORTHO_FOOTPRINT_H

#include <tidop/core/path.h>
#include <tidop/core/task.h>
#include <tidop/vect/vectwriter.h>
#include <tidop/geospatial/crs.h>

#include "graphos/core/image.h"
#include "graphos/core/camera/Camera.h"

namespace graphos
{

/*!
 * \brief Footprint
 */
class Footprint
	: public tl::TaskBase
{

public:

	Footprint(const std::vector<Image> &images,
						const std::map<int, Camera> &cameras,
		        const tl::Path &dtm,
		        const tl::geospatial::Crs &crs,
		        const tl::Path &footprint);
	~Footprint();
	
// Heredado vía TaskBase

private:

	void execute(tl::Progress *progressBar = nullptr) override;

private:

	std::vector<Image> mImages;
	std::map<int, Camera> mCameras;
	tl::Path mDtm;
	tl::geospatial::Crs mCrs;
	std::unique_ptr<tl::VectorWriter> mFootprintWriter;
};


} // End namespace graphos

#endif // GRAPHOS_CORE_ORTHO_FOOTPRINT_H
