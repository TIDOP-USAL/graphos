#ifndef GRAPHOS_DENSE_MODEL_H
#define GRAPHOS_DENSE_MODEL_H

#include <QObject>

#include "graphos/ui/densification/DensificationModel.h"

namespace graphos
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

// Model interface

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

} // End namespace graphos

#endif // GRAPHOS_DENSE_MODEL_H
