#ifndef INSPECTOR_DENSE_MODEL_INTERFACE_H
#define INSPECTOR_DENSE_MODEL_INTERFACE_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

class Densification;

namespace ui
{

class DensificationModel
  : public IModel
{

  Q_OBJECT

public:

  DensificationModel(QObject *parent = nullptr) : IModel(parent) {}
  ~DensificationModel() override = default;

  virtual std::shared_ptr<Densification> densification() const = 0;
  virtual QString projectFolder() const = 0;
  virtual QString imageDirectory() const = 0;
  virtual QString reconstructionPath() const = 0;
  virtual bool useCuda() const = 0;
  virtual bool existDenseModel() const = 0;

public slots:

  virtual void setDensification(const std::shared_ptr<Densification> &densification) = 0;
  virtual void setDenseModel(const QString &denseModel) = 0;
};


} // End namespace ui

} // End namespace inspector

#endif // INSPECTOR_DENSE_MODEL_INTERFACE_H
