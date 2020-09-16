#ifndef INSPECTOR_UTILS_H
#define INSPECTOR_UTILS_H


#include "inspector/inspector_global.h"

#include <tidop/core/defs.h>

#include <opencv2/photo.hpp>

#include <QString>
#include <QImageReader>

#include <iostream>
#include <fstream>
#include <vector>


namespace inspector
{

/*----------------------------------------------------------------*/

/*!
 * \brief Convert QImage to cv::Mat
 * \param[in] image QImage
 * \return cv::Mat
 */
INSPECTOR_EXPORT cv::Mat qImageToCvMat(const QImage &image);

/*!
 * \brief Convert cv::Mat to QImage
 * \param[in] image cv::Mat
 * \return QImage
 */
INSPECTOR_EXPORT QImage cvMatToQImage(const cv::Mat &image);


INSPECTOR_EXPORT cv::Mat convertToGray(const cv::Mat &image);

INSPECTOR_EXPORT QSize cvSizeToQSize(const cv::Size &size);
INSPECTOR_EXPORT cv::Size qSizeToCvSize(const QSize &size);

/*----------------------------------------------------------------*/

/*!
 * \brief Comprueba si el procesamiento por cuda está disponible
 * Se comprueba que:
 * - Se haya compilado colmap con soporte cuda
 * - El equipo cuente con una GPU compatible con cuda
 * \return True si se cumplen las dos opciones anteriores
 */
INSPECTOR_EXPORT bool cudaEnabled();  ///TODO: mover a un fichero de utilidades
INSPECTOR_EXPORT bool cudaEnabled(double minDriver, double minCapability);


TL_TODO("Solución un tanto provisional...")
class INSPECTOR_EXPORT LogStreamBuf
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


class INSPECTOR_EXPORT LogStream
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


} // end namespace inspector

#endif // INSPECTOR_UTILS_H
