#include "utils.h"

#include <tidop/core/messages.h>

#ifdef HAVE_CUDA
#include <cuda_runtime.h>
#endif

#include <cstring>
#include <ostream>

namespace inspector
{

cv::Mat qImageToCvMat(const QImage &image)
{
  QImage::Format qt_format = image.format();

  cv::Mat img;
  if (
#if QT_VERSION > QT_VERSION_CHECK(5, 5, 0)
      qt_format == QImage::Format_Grayscale8 ||
#endif
      qt_format == QImage::Format_Indexed8){

    img = cv::Mat(image.height(), image.width(), CV_8UC1,
                                const_cast<uchar*>(image.bits()),
                                static_cast<size_t>(image.bytesPerLine())
                                ).clone();
  }
#if QT_VERSION > QT_VERSION_CHECK(5, 13, 0)
  else if (qt_format == QImage::Format_Grayscale16) {
    img = cv::Mat(image.height(), image.width(), CV_16UC1,
                                const_cast<uchar*>(image.bits()),
                                static_cast<size_t>(image.bytesPerLine())
                                ).clone();
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
                  const_cast<uchar*>(image.constBits()),
                  static_cast<size_t>(image.bytesPerLine())).clone();
    cvtColor(img, img, cv::COLOR_RGB2BGR);

  }
#if QT_VERSION > QT_VERSION_CHECK(5, 2, 0)
  else if (qt_format == QImage::Format_RGBA8888 || qt_format == QImage::Format_RGBX8888 || qt_format == QImage::Format_RGBA8888_Premultiplied) {
    img = cv::Mat(image.height(), image.width(), CV_8UC4,
                  const_cast<uchar*>(image.constBits()),
                  static_cast<size_t>(image.bytesPerLine())).clone();
    cvtColor(img, img, cv::COLOR_RGBA2BGRA);
  }
#endif
  else if (qt_format == QImage::Format_RGB32){
    img = cv::Mat(image.height(), image.width(), CV_8UC4,
                  const_cast<uchar*>(image.constBits()),
                  static_cast<size_t>(image.bytesPerLine())).clone();
    cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
  } else if (qt_format == QImage::Format_ARGB32 || qt_format == QImage::Format_ARGB32_Premultiplied) {
    img = cv::Mat(image.height(), image.width(), CV_8UC4,
                  const_cast<uchar*>(image.constBits()),
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
  if (image.channels() == 1){
    format = QImage::Format_Grayscale8;
    if (image.depth() == CV_8U) {
      image.copyTo(aux);
    } else if (image.depth() == CV_8S || 
               image.depth() == CV_16U) {
      //format = QImage::Format_RGB888;
      //cv::Mat temp;
      cv::normalize(image, aux, 0., 255., cv::NORM_MINMAX, CV_8U);
      //cv::applyColorMap(temp, aux, cv::COLORMAP_HOT);
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

cv::Mat convertToGray(const cv::Mat &image)
{
  cv::Mat temp;
  if (image.channels() >= 3) {
    cv::Mat color_boost;
    cv::decolor(image, temp, color_boost);
    color_boost.release();
  } else {
    image.copyTo(temp);
  }
  return temp;
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
  if(err_cuda == cudaSuccess){
    for (int id = 0; id < count; id++){
      cudaDeviceProp propiedades;
      err_cuda = cudaGetDeviceProperties(&propiedades, id);
      if(err_cuda == cudaSuccess){
        bUseGPU = true;
        break;
      } else {
        msgError("%s: %s", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
      }
    }
  } else {
    msgError("%s: %s", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
  }

#else
  msgWarning("No Cuda support enabled. The processing time will increase significantly");
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
        msgError("%s: %s", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
      }
    }

    if (bUseGPU) {
      msgInfo("Detected %i CUDA Capable %s", count, count == 1 ? "device" : "devices");
      for (int id = 0; id < count; id++) {
        cudaDeviceProp propiedades;
        err_cuda = cudaGetDeviceProperties(&propiedades, id);
        if (err_cuda == cudaSuccess) {
          msgInfo("Device %i: %s", id, propiedades.name);
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

            msgInfo("  Cuda Driver Version / Runtime Version             %i.%i / %i.%i", driver_version_major, driver_version_minor, runtime_version_major, runtime_version_minor);
          }

          msgInfo("  Cuda Capability Major / Minor version number      %i.%i", propiedades.major, propiedades.minor);
        
          if (runtime_version_major < minDriver || propiedades.major < minCapability) {
            bUseGPU = false;
            msgWarning("No cuda compatibility device found. The processing time will increase significantly");
          }
        
        }
      }
    }

  } else {
    msgError("%s: %s", cudaGetErrorName(err_cuda), cudaGetErrorString(err_cuda));
  }

#else
  msgWarning("No Cuda support enabled. The processing time will increase significantly");
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
  }
  else {
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

} // end namespace inspector
