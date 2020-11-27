#ifndef INSPECTOR_DTM_MODEL_INTERFACE_H
#define INSPECTOR_DTM_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"


namespace inspector
{


namespace ui
{

class DtmModel
  : public IModel
{

public:

  DtmModel(QObject *parent = nullptr) : IModel(parent) {}
  ~DtmModel() override = default;

  virtual std::array<double, 3> offset() const = 0;
  virtual QString projectPath() const = 0;
  virtual QString denseModel() const = 0;

  //virtual std::shared_ptr<Feature> featureExtractor() const = 0;
  //virtual void setCurrentDtmMethod(const std::shared_ptr<Feature> &featureExtractor) = 0;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_DTM_MODEL_INTERFACE_H
