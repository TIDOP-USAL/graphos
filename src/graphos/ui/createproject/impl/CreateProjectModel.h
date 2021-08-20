#ifndef GRAPHOS_CREATE_PROJECT_MODEL_H
#define GRAPHOS_CREATE_PROJECT_MODEL_H

#include "graphos/ui/createproject/CreateProjectModel.h"

namespace graphos
{

class Project;

namespace ui
{


class CreateProjectModelImp
  : public CreateProjectModel
{

  Q_OBJECT

public:

  explicit CreateProjectModelImp(Project *project,
                                 QObject *parent = nullptr);
  ~CreateProjectModelImp() override;

signals:



// CreateProjectModel interface

public:

  QString projectName() const override;
  QString projectDescription() const override;
  QString projectFolder() const override;
  QString projectPath() const override;

public slots:

  void setProjectName(const QString &name) override;
  void setProjectDescription(const QString &projectDescription) override;
  void setProjectFolder(const QString &dir) override;
  void setDatabase(const QString &database) override;
  void saveAs(const QString &file) override;

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
  QString mPrjFile;
  bool bUnsavedChanges;

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_CREATE_PROJECT_MODEL_H

