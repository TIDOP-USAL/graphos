#ifndef INSPECTOR_DENSE_MODEL_H
#define INSPECTOR_DENSE_MODEL_H

#include <QObject>

#include "inspector/ui/densification/DensificationModel.h"

namespace inspector
{

class Project;

namespace ui
{

class DensificationModelImp
  : public DensificationModel
{
  Q_OBJECT

public:

  DensificationModelImp(Project *project,
                        QObject *parent = nullptr);
  ~DensificationModelImp() override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

// DensificationModel interface

public:

  std::shared_ptr<Densification> densification() const override;
  QString projectFolder() const override;
  QString reconstructionPath() const override;
  bool useCuda() const override;
  bool existDenseModel() const override;

public slots:

  void setDensification(const std::shared_ptr<Densification> &densification) override;
  void setDenseModel(const QString &denseModel) override;

protected:

  Project *mProject;

};

} // End namespace ui

} // End namespace inspector

#endif // GRAPHOS_DENSE_MODEL_H
