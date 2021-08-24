#ifndef GRAPHOS_DTM_MODEL_INTERFACE_H
#define GRAPHOS_DTM_MODEL_INTERFACE_H

#include "graphos/interfaces/mvp.h"


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
  virtual QString projectPath() const = 0;
  virtual QString denseModel() const = 0;
  virtual QString crs() const = 0;

  virtual std::shared_ptr<Dtm> dtmMethod() const = 0;
  virtual void setDtmMethod(const std::shared_ptr<Dtm> &dtm) = 0;
  virtual QString dtmPath() const = 0;
  virtual void setDtmPath(const QString &dtmPath) = 0;
};

} // namespace graphos

#endif // GRAPHOS_DTM_MODEL_INTERFACE_H
