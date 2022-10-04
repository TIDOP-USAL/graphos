#ifndef GRAPHOS_DTM_MODEL_INTERFACE_H
#define GRAPHOS_DTM_MODEL_INTERFACE_H

#include "graphos/core/mvp.h"

#include <tidop/core/path.h>

namespace graphos
{

class Dtm;

class DtmModel
  : public Model
{

public:

  DtmModel(QObject *parent = nullptr) : Model(parent) {}
  ~DtmModel() override = default;

  virtual std::array<double, 3> offset() const = 0;
  virtual tl::Path projectPath() const = 0;
  virtual tl::Path denseModel() const = 0;
  virtual QString crs() const = 0;

  virtual std::shared_ptr<Dtm> dtmMethod() const = 0;
  virtual void setDtmMethod(const std::shared_ptr<Dtm> &dtm) = 0;
  virtual tl::Path dtmPath() const = 0;
  virtual void setDtmPath(const tl::Path &dtmPath) = 0;
};

} // namespace graphos

#endif // GRAPHOS_DTM_MODEL_INTERFACE_H
