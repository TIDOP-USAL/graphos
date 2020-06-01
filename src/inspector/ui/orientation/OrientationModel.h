#ifndef INSPECTOR_ORIENTATION_MODEL_H
#define INSPECTOR_ORIENTATION_MODEL_H

#include "inspector/ui/orientation/Orientation.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class OrientationModelImp
  : public OrientationModel
{

public:

  OrientationModelImp(Project *project,
                      QObject *parent = nullptr);
  ~OrientationModelImp() override = default;

// OrientationModel interface

public:

  bool refinePrincipalPoint() const override;
  void setRefinePrincipalPoint(bool refine) override;
  void setSparseModel(const QString &sparseModel) override;
  QString database() const override;
  QString imagePath() const override;
  QString projectPath() const override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_ORIENTATION_MODEL_H
