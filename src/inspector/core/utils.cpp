#include "utils.h"

#include <tidop/core/messages.h>

#ifdef HAVE_CUDA
#include <cuda_runtime.h>
#endif

#include <cstring>
#include <ostream>

namespace inspector
{

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
