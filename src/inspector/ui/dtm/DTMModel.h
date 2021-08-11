#ifndef INSPECTOR_DTM_MODEL_INTERFACE_H
#define INSPECTOR_DTM_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{

class Dtm;

namespace ui
{

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

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_DTM_MODEL_INTERFACE_H
