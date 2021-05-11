#ifndef INSPECTOR_CSF_H
#define INSPECTOR_CSF_H

#include "inspector/inspector_global.h"

#include <map>
#include <memory>

#include <QString>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>


namespace inspector
{

namespace internal
{
class Csf;
}

class INSPECTOR_EXPORT Csf
{

public:

  Csf();
  ~Csf();

  bool sloopSmooth() const;
  void setSloopSmooth(bool sloopSmooth);
  double timeStep() const;
  void setTimeStep(double timeStep);
  double classThreshold() const;
  void setClassThreshold(double classThreshold);
  double clothResolution() const;
  void setClothResolution(double clothResolution);
  int rigidness() const;
  void setRigidness(int rigidness);
  int iterations() const;
  void setIterations(int iterations);

  void filter(const std::string &pointCloud, 
              const std::string &ground, 
              const std::string &outGround);

private:



private:

  std::unique_ptr<internal::Csf> mCSF;
};


} // namespace inspector

#endif // INSPECTOR_CSF_H
