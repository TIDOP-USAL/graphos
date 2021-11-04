#ifndef INSPECTOR_NEW_PROJECT_PRESENTER_H
#define INSPECTOR_NEW_PROJECT_PRESENTER_H

#include <memory>

#include <QObject>

#include "inspector/components/NewProject.h"

namespace inspector
{

class HelpDialog;

namespace ui
{
	
class NewProjectView;
class ProjectModel;

class NewProjectPresenterImp
  : public NewProjectPresenter
{
  Q_OBJECT

public:

  NewProjectPresenterImp(NewProjectView *view,
                         ProjectModel *model);
  ~NewProjectPresenterImp() override = default;

// INewProjectPresenter interface

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

  NewProjectView *mView;
  ProjectModel *mProjectModel;
  QString mProjectsDefaultPath;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_NEW_PROJECT_PRESENTER_H
