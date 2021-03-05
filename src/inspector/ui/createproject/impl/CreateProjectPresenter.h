#ifndef INSPECTOR_CREATE_PROJECT_PRESENTER_H
#define INSPECTOR_CREATE_PROJECT_PRESENTER_H

#include "inspector/ui/createproject/CreateProjectPresenter.h"

namespace inspector
{

class HelpDialog;

namespace ui
{
	
class CreateProjectView;
class CreateProjectModel;

class CreateProjectPresenterImp
  : public CreateProjectPresenter
{
  Q_OBJECT

public:

  CreateProjectPresenterImp(CreateProjectView *view,
                            CreateProjectModel *model);
  ~CreateProjectPresenterImp() override = default;

// CreateProjectPresenter interface

protected slots:

  void saveProject() override;
  void discartProject() override;
  void checkProjectName() const override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

// Métodos privados

private:

  QString projectFolder() const;
  void createProjectFolderIfNoExist(const QString &projectFolder);
  QString databasePath(const QString &projectFolder) const;
  QString projectPath(const QString &projectFolder) const;

private:

  CreateProjectView *mView;
  CreateProjectModel *mModel;
  QString mProjectsDefaultPath;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_CREATE_PROJECT_PRESENTER_H
