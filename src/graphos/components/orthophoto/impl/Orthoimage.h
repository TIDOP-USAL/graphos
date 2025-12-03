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
 * \class Orthoimage
 * \brief Generates an orthorectified image in the specified output CRS using camera orientation and DSM data.
 *
 * This class performs the orthorectification of a single input image. It projects
 * each pixel from the image into the target coordinate reference system (CRS),
 * typically a projected system such as UTM. The transformation uses the
 * camera orientation and the Digital Surface Model (DSM) provided by the
 * associated Orthorectification object.
 *
 * Although the project data (camera poses, DSM) are defined in the local ENU
 * system, the orthophoto is generated directly in the specified output CRS.
 *
 * The process supports multiple interpolation methods and can be executed
 * on CPU or using CUDA acceleration.
 *
 * \see Orthorectification, OrthophotoTask, ZBuffer
 */
class Orthoimage
{

public:

	/*!
	 * \brief Constructs an Orthoimage object.
	 * \param[in] image Path to the input image to be orthorectified.
	 * \param[in] orthorectification Pointer to the Orthorectification object providing DSM and camera data.
	 * \param[in] enuCrs CRS string corresponding to the local ENU coordinate system of the project.
	 * \param[in] crs Target coordinate reference system (CRS) for the output orthophoto.
	 * \param[in] sizeOrtho Size of the output orthophoto in pixels.
	 * \param[in] orthoGeoreference Affine transform defining the orthophoto georeference in the output coordinate system of the project.
	 * \param[in] interpolation Interpolation method ("NEAREST", "BILINEAR", "CUBIC").
	 * \param[in] cuda True to enable CUDA-based processing, false for CPU.
	 */
	Orthoimage(const tl::Path &image,
			   Orthorectification *orthorectification,
			   const std::string &enuCrs,
			   const std::string &crs,
			   const tl::Size<int> &sizeOrtho,
			   const tl::Affine<double, 2> &orthoGeoreference,
			   const std::string &interpolation = "BILINEAR",
			   bool cuda = false);

	~Orthoimage();

	/*!
	 * \brief Runs the orthorectification process and writes the output orthophoto.
	 * \param[in] ortho Output file path for the orthophoto.
	 * \param[in] visibilityMap Optional visibility map to restrict valid terrain areas.
	 *
	 * The function reads the input image, projects it into the target CRS
	 * according to the DSM and camera parameters, performs interpolation,
	 * and writes the resulting orthophoto to disk.
	 */
	void run(const tl::Path &ortho,
			 const cv::Mat &visibilityMap = cv::Mat());

protected:

	auto readImage() -> cv::Mat;
	void normalizeImage(cv::Mat &mat) const;

private:

	tl::Path mImage;
	Orthorectification *mOrthorectification;
	std::string mEnuCrs;
	std::string mCrs;
    tl::Size<int> mSizeOrtho;
	tl::Affine<double, 2> mGeoreference;
	//tl::Window<tl::Point<double>> mWindowOrthoTerrain;
	std::string mInterpolation;
	bool bCuda;
	bool mReadWithOpenCV;
	tl::DataType mDataType;

};



} // End namespace graphos


#endif // GRAPHOS_CORE_ORTHO_ORTHOIMAGE_H
