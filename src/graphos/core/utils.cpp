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

#include "graphos/core/utils.h"
#include "graphos/core/Pdf.h"
#include "graphos/core/ply.h"

#include <tidop/core/msg/message.h>
#include <tidop/core/chrono.h>
#include <tidop/geospatial/util.h>

#ifdef GRAPHOS_GUI
#include <FileIOFilter.h>
#include <ccPointCloud.h>
#include <ccGenericMesh.h>
#include <PlyFilter.h>
#include <ccHObjectCaster.h>
#endif // GRAPHOS_GUI

#ifdef HAVE_CUDA
#include <cuda_runtime.h>
#endif
#ifdef HAVE_OPENCV_CUDAIMGPROC
#include <opencv2/cudaimgproc.hpp>
#endif // HAVE_OPENCV_CUDAIMGPROC
#ifdef HAVE_OPENCV_CUDAARITHM
#include <opencv2/cudaarithm.hpp>
#endif // HAVE_OPENCV_CUDAARITHM
#ifdef HAVE_OPENCV_CUDAWARPING
#include <opencv2/cudawarping.hpp>
#endif // HAVE_OPENCV_CUDAWARPING

#include <cstring>
#include <ostream>

namespace graphos
{

cv::Mat qImageToCvMat(const QImage &image)
{
    QImage::Format qt_format = image.format();

    cv::Mat img;
    if (
#if QT_VERSION > QT_VERSION_CHECK(5, 5, 0)
        qt_format == QImage::Format_Grayscale8 ||
#endif
        qt_format == QImage::Format_Indexed8) {

        img = cv::Mat(image.height(), image.width(), CV_8UC1,
                      const_cast<uchar *>(image.bits()),
                      static_cast<size_t>(image.bytesPerLine())).clone();
    }
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
    else if (qt_format == QImage::Format_Grayscale16) {
        img = cv::Mat(image.height(), image.width(), CV_16UC1,
                      const_cast<uchar *>(image.bits()),
                      static_cast<size_t>(image.bytesPerLine())).clone();
    }
#endif
    else if (qt_format == QImage::Format_RGB888) {

        /// Opción 1
    //      QImage swapped = image.rgbSwapped();
    //      img = cv::Mat(swapped.height(), swapped.width(), CV_8UC3,
    //                                  const_cast<uchar*>(swapped.bits()),
    //                                  static_cast<size_t>(swapped.bytesPerLine())
    //                                  ).clone();

        /// Opción 2
        img = cv::Mat(image.height(), image.width(), CV_8UC3,
                      const_cast<uchar *>(image.constBits()),
                      static_cast<size_t>(image.bytesPerLine())).clone();
        cvtColor(img, img, cv::COLOR_RGB2BGR);

    }
#if QT_VERSION > QT_VERSION_CHECK(5, 2, 0)
    else if (qt_format == QImage::Format_RGBA8888 || qt_format == QImage::Format_RGBX8888 || qt_format == QImage::Format_RGBA8888_Premultiplied) {
        img = cv::Mat(image.height(), image.width(), CV_8UC4,
                      const_cast<uchar *>(image.constBits()),
                      static_cast<size_t>(image.bytesPerLine())).clone();
        cvtColor(img, img, cv::COLOR_RGBA2BGRA);
    }
#endif
    else if (qt_format == QImage::Format_RGB32) {
        img = cv::Mat(image.height(), image.width(), CV_8UC4,
                      const_cast<uchar *>(image.constBits()),
                      static_cast<size_t>(image.bytesPerLine())).clone();
        cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
    } else if (qt_format == QImage::Format_ARGB32 || qt_format == QImage::Format_ARGB32_Premultiplied) {
        img = cv::Mat(image.height(), image.width(), CV_8UC4,
                      const_cast<uchar *>(image.constBits()),
                      static_cast<size_t>(image.bytesPerLine())).clone();
    } else if (qt_format == QImage::Format_RGB16) {

    }
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
    else if (qt_format == QImage::Format_RGBX64 || qt_format == QImage::Format_RGBA64 || qt_format == QImage::Format_RGBA64_Premultiplied) {

    }
#endif
    return img;
}

QImage cvMatToQImage(const cv::Mat &image)
{
    cv::Mat aux;
    QImage::Format format;
    if (image.channels() == 1) {
        format = QImage::Format_Grayscale8;
        if (image.depth() == CV_8U) {
            image.copyTo(aux);
        } else if (image.depth() == CV_8S ||
                   image.depth() == CV_16U) {
            //format = QImage::Format_RGB888;
            //cv::Mat temp;
            cv::normalize(image, aux, 0., 255., cv::NORM_MINMAX, CV_8U);
            //cv::applyColorMap(temp, aux, cv::COLORMAP_HOT);
        } else if (image.depth() == CV_32F ||
                   image.depth() == CV_64F) {
            cv::Mat temp;
            cv::Mat mask_image = cv::Mat::zeros(image.rows, image.cols, CV_8U);
            mask_image.setTo(cv::Scalar::all(255), image > -9999.);
            cv::normalize(image, temp, 0., 255., cv::NORM_MINMAX, CV_8U, mask_image);
            cv::applyColorMap(temp, aux, cv::COLORMAP_JET);
            format = QImage::Format_RGB888;
            cv::cvtColor(aux, aux, cv::COLOR_BGR2RGB);
        }

    } else if (image.channels() == 3) {
        format = QImage::Format_RGB888;
        cv::cvtColor(image, aux, cv::COLOR_BGR2RGB);
    } else if (image.channels() == 4) {
        format = QImage::Format_RGBA8888;
        cv::cvtColor(image, aux, cv::COLOR_BGRA2RGB);
    } else {
        format = QImage::Format_Invalid;
    }

    QImage q_image;

    if (format != QImage::Format_Invalid)
        q_image = QImage(aux.data, aux.cols, aux.rows, static_cast<int>(aux.step), format).copy();

    return q_image;
}

void convertToGray(const cv::Mat &in, cv::Mat &out, bool useCuda)
{
    //cv::Mat temp;
    //if (image.channels() >= 3) {
    //    cv::Mat color_boost;
    //    cv::decolor(image, temp, color_boost);
    //    color_boost.release();
    //} else {
    //    image.copyTo(temp);
    //}
    //return temp;
#ifdef HAVE_OPENCV_CUDAIMGPROC
    if (useCuda) {
        cv::cuda::GpuMat gImgIn(in);
        cv::cuda::GpuMat gImgGray;
        cv::cuda::cvtColor(gImgIn, gImgGray, cv::COLOR_BGR2GRAY);
        gImgGray.download(out);
    } else {
#endif
        cv::Mat temp;
        cv::cvtColor(in, temp, cv::COLOR_BGR2GRAY);
        temp.copyTo(out);
#ifdef HAVE_OPENCV_CUDAIMGPROC
    }
#endif
}

void normalizeImage(const cv::Mat &in, cv::Mat &out, bool useCuda, cv::InputArray &mask)
{
    if (in.depth() != CV_8U) {
#ifdef HAVE_OPENCV_CUDAARITHM
        if (useCuda) {
            cv::cuda::GpuMat gImgIn(in);
            cv::cuda::GpuMat gMask(mask);
            cv::cuda::GpuMat gImgOut;
            cv::cuda::normalize(gImgIn, gImgOut, 0., 255., cv::NORM_MINMAX, CV_8U, gMask);
            gImgOut.download(out);
        } else {
#endif
            cv::normalize(in, out, 0., 255., cv::NORM_MINMAX, CV_8U, mask);
#ifdef HAVE_OPENCV_CUDAARITHM
        }
#endif
    } else {
        out = in;
    }
}

void resizeImage(const cv::Mat &in, cv::Mat &out, const cv::Size &size, bool useCuda)
{
#ifdef HAVE_OPENCV_CUDAWARPING
    if (useCuda) {
        cv::cuda::GpuMat gImgIn(in);
        cv::cuda::GpuMat gImgResize;
        cv::cuda::resize(gImgIn, gImgResize, size);
        gImgResize.download(out);
    } else {
#endif
        cv::resize(in, out, size);
#ifdef HAVE_OPENCV_CUDAWARPING
    }
#endif
}

QSize cvSizeToQSize(const cv::Size &size)
{
    return QSize(size.width, size.height);
}

cv::Size qSizeToCvSize(const QSize &size)
{
    return cv::Size(size.width(), size.height());
}




bool cudaEnabled()
{
    bool bUseGPU = false;
#ifdef HAVE_CUDA

    /// Se obtienen el número de dispositivos compatibles
    int count;
    cudaError_t err_cuda = cudaGetDeviceCount(&count);
    if (err_cuda == cudaSuccess) {
        for (int id = 0; id < count; id++) {
            cudaDeviceProp propiedades;
            err_cuda = cudaGetDeviceProperties(&propiedades, id);
            if (err_cuda == cudaSuccess) {
                bUseGPU = true;
                break;
            } else {
                tl::Message::error("{}: {}", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
            }
        }
    } else {
        tl::Message::error("{}: {}", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
    }

#else
    tl::Message::warning("No Cuda support enabled. The processing time will increase significantly");
#endif

    return bUseGPU;
}

TL_EXPORT bool cudaEnabled(double minDriver, double minCapability)
{
    bool bUseGPU = false;
#ifdef HAVE_CUDA

    /// Se obtienen el número de dispositivos compatibles
    int count;
    cudaError_t err_cuda = cudaGetDeviceCount(&count);
    if (err_cuda == cudaSuccess) {
        for (int id = 0; id < count; id++) {
            cudaDeviceProp propiedades;
            err_cuda = cudaGetDeviceProperties(&propiedades, id);
            if (err_cuda == cudaSuccess) {
                bUseGPU = true;
                break;
            } else {
                tl::Message::error("[{}] {}", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
            }
        }

        if (bUseGPU) {
            tl::Message::info("Detected {} CUDA Capable {}", count, count == 1 ? "device" : "devices");
            for (int id = 0; id < count; id++) {
                cudaDeviceProp propiedades;
                err_cuda = cudaGetDeviceProperties(&propiedades, id);
                if (err_cuda == cudaSuccess) {
                    tl::Message::info("Device {}: {}", id, propiedades.name);
                    int runtimeVersion = 0;
                    int driverVersion = 0;
                    err_cuda = cudaRuntimeGetVersion(&runtimeVersion);
                    if (err_cuda == cudaSuccess) {
                        err_cuda = cudaDriverGetVersion(&driverVersion);
                    }
                    int runtime_version_major = 0;
                    int runtime_version_minor = 0;
                    int driver_version_major = 0;
                    int driver_version_minor = 0;

                    if (err_cuda == cudaSuccess) {

                        runtime_version_major = runtimeVersion / 1000;
                        runtime_version_minor = static_cast<int>(std::round((runtimeVersion / 1000. - std::round(runtimeVersion / 1000.)) * 100));
                        driver_version_major = driverVersion / 1000;
                        driver_version_minor = static_cast<int>(std::round((driverVersion / 1000. - std::round(driverVersion / 1000.)) * 100));

                        tl::Message::info("  Cuda Driver Version / Runtime Version             {}.{} / {}.{}", driver_version_major, driver_version_minor, runtime_version_major, runtime_version_minor);
                    }

                    tl::Message::info("  Cuda Capability Major / Minor version number      {}.{}", propiedades.major, propiedades.minor);

                    if (runtime_version_major < minDriver || propiedades.major < minCapability) {
                        bUseGPU = false;
                    }

                }
            }
        }

    } else {
        tl::Message::error("[{}] {}", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
    }

    if (!bUseGPU) {
        tl::Message::warning("No cuda compatibility device found. The processing time will increase significantly");
    }

#else
    tl::Message::warning("No Cuda support enabled. The processing time will increase significantly");
#endif

    return bUseGPU;
}




LogStreamBuf::LogStreamBuf(std::streambuf *sb)
  : std::streambuf(),
    mStreamBuf(sb),
    mVectChar(0)
{
}

int LogStreamBuf::sync()
{
    int const r = mStreamBuf->pubsync();
    //std::stringstream buf;
    for (auto &_char : mVectChar)
        //buf << _char;
        printf("%c", _char);
    //std::string _string = buf.str();
    mVectChar.clear();
    //Log &log = Log::getInstance();
    //Console::getInstance().printMessage(_string.c_str());
    //log.write(_string.c_str());
    return r == 0 ? 0 : -1;
}

int LogStreamBuf::overflow(int c)
{
    if (c == EOF) {
        return !EOF;
    } else {
        char t = static_cast<char>(c);
        int const r = mStreamBuf->sputc(t);
        mVectChar.push_back(t);
        return r == EOF ? EOF : c;
    }
}


LogStream::LogStream()
  : std::ostream(nullptr),
    mOfstream(),
    mLogStreamBuf(mOfstream.rdbuf())
{
    this->init(&mLogStreamBuf);
}

void LogStream::open(const std::string &s, ios_base::openmode mode)
{
    mOfstream.open(s, mode);
}

bool LogStream::isOpen() const
{
    return mOfstream.is_open();
}

void LogStream::close()
{
    mOfstream.close();
}

std::streambuf *LogStream::rdbuf()
{
    return &mLogStreamBuf;
}




void openPdf(const QString &pdf)
{
    Pdf pdf_file(pdf);
    pdf_file.show();
}

tl::Degrees<double> formatDegreesFromExif(const std::string &exifAngle, const std::string &ref)
{
    auto angle = tl::convertStringTo<double>(exifAngle);
    if (ref == "S" || ref == "W") angle = -angle;
    return {angle};
}




void transformModel(const tl::Matrix<double> &transform, const std::string &model)
{
#ifdef GRAPHOS_GUI
    CC_FILE_ERROR result = CC_FERR_NO_ERROR;
    ccHObject group;

    try {

        PlyFilter filter;
        FileIOFilter::LoadParameters parameters;
        parameters.alwaysDisplayLoadDialog = false;
        parameters.shiftHandlingMode = ccGlobalShiftManager::NO_DIALOG_AUTO_SHIFT;
        parameters.parentWidget = nullptr;
        result = filter.loadFile(QString::fromStdString(model), group, parameters);
        
        TL_ASSERT(result == CC_FERR_NO_ERROR, "Open Ply error");

        ccHObject::Container clouds;
        group.filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

        //TL_ASSERT(clouds.size() == 1, "Error");

        ccHObject *model_3d = clouds.at(0);

        bool lockedVertices;
        ccGenericPointCloud *cloud = ccHObjectCaster::ToGenericPointCloud(model_3d, &lockedVertices);
        ccGLMatrix cc_rot;
        float *mat = cc_rot.data();

        mat[0] = static_cast<float>(transform[0][0]);
        mat[1] = static_cast<float>(transform[1][0]);
        mat[2] = static_cast<float>(transform[2][0]);

        mat[4] = static_cast<float>(transform[0][1]);
        mat[5] = static_cast<float>(transform[1][1]);
        mat[6] = static_cast<float>(transform[2][1]);

        mat[8] = static_cast<float>(transform[0][2]);
        mat[9] = static_cast<float>(transform[1][2]);
        mat[10] = static_cast<float>(transform[2][2]);
       
        CCVector3 t(static_cast<PointCoordinateType>(transform[0][3]),
                    static_cast<PointCoordinateType>(transform[1][3]),
                    static_cast<PointCoordinateType>(transform[2][3]));
        cc_rot.setTranslation(t);

        cloud->applyRigidTransformation(cc_rot);

        FileIOFilter::SaveParameters save_parameters;
        save_parameters.alwaysDisplaySaveDialog = false;
        save_parameters.parentWidget = nullptr;
        result = filter.saveToFile(group.getChild(0), QString::fromStdString(model), save_parameters);

        TL_ASSERT(result == CC_FERR_NO_ERROR, "Save Ply error");

    } catch (...) {
        TL_THROW_EXCEPTION_WITH_NESTED("Transform model error");
    }
#endif // GRAPHOS_GUI
}

QString enuCrsToEpsg(const QString &enuCRS)
{
    auto v = tl::split<std::string>(enuCRS.toStdString(), ';');
    auto zone = tl::utmZoneFromLonLat(tl::stringToNumber<double>(v.at(1)), tl::stringToNumber<double>(v.at(2)));
    QString epsg_code("EPSG:326");
    epsg_code.append(QString::number(zone.first));
    return epsg_code;
}


} // end namespace graphos
