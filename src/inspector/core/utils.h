#ifndef INSPECTOR_UTILS_H
#define INSPECTOR_UTILS_H


#include "inspector/inspector_global.h"

#include <tidop/core/defs.h>

#include <iostream>
#include <fstream>
#include <vector>

namespace inspector
{

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
