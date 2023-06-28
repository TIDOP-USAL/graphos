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

#ifndef GRAPHOS_CORE_UTILS_H
#define GRAPHOS_CORE_UTILS_H


#include "graphos/graphos_global.h"

#include <tidop/core/defs.h>
#include <tidop/math/angles.h>
#include <tidop/math/algebra/matrix.h>

#include <opencv2/photo.hpp>

#include <QString>
#include <QImageReader>

#include <iostream>
#include <fstream>
#include <vector>


namespace graphos
{

/*----------------------------------------------------------------*/

/*!
 * \brief Convert QImage to cv::Mat
 * \param[in] image QImage
 * \return cv::Mat
 */
cv::Mat qImageToCvMat(const QImage &image);

/*!
 * \brief Convert cv::Mat to QImage
 * \param[in] image cv::Mat
 * \return QImage
 */
QImage cvMatToQImage(const cv::Mat &image);


cv::Mat convertToGray(const cv::Mat &image);

QSize cvSizeToQSize(const cv::Size &size);
cv::Size qSizeToCvSize(const QSize &size);

/*----------------------------------------------------------------*/

/*!
 * \brief Comprueba si el procesamiento por cuda está disponible
 * Se comprueba que:
 * - Se haya compilado colmap con soporte cuda
 * - El equipo cuente con una GPU compatible con cuda
 * \return True si se cumplen las dos opciones anteriores
 */
bool cudaEnabled();  ///TODO: mover a un fichero de utilidades
bool cudaEnabled(double minDriver, double minCapability);


class LogStreamBuf
  : public std::streambuf
{

protected:

  std::streambuf *mStreamBuf;
  std::vector<char> mVectChar;

public:

  LogStreamBuf(std::streambuf *sb);

protected:

  int sync() override;
  int overflow(int c) override;
};


class LogStream
  : public std::ostream
{

protected:

  std::ofstream mOfstream;
  LogStreamBuf mLogStreamBuf;

public:

  LogStream();

  void open(const std::string &s, ios_base::openmode mode = ios_base::in | ios_base::out | ios_base::app /*ios_base::trunc*/);
  bool isOpen() const;
  void close();
  std::streambuf *rdbuf();

};

void openPdf(const QString &pdf);

tl::math::Degrees<double> formatDegreesFromExif(const std::string &exifAngle, const std::string &ref);




void transformModel(const tl::math::Matrix<double> &transform, const std::string &model);


} // end namespace graphos

#endif // GRAPHOS_CORE_UTILS_H
