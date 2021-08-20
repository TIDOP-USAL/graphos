#ifndef GRAPHOS_CSF_H
#define GRAPHOS_CSF_H

#include "graphos/graphos_global.h"

#include <map>
#include <memory>

#include <QString>

#include <tidop/core/defs.h>
#include <tidop/core/flags.h>


namespace graphos
{

namespace internal
{
class Csf;
}

class GRAPHOS_EXPORT Csf
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


} // namespace graphos

#endif // GRAPHOS_CSF_H
