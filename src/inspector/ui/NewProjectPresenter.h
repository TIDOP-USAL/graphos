#ifndef INSPECTOR_NEW_PROJECT_PRESENTER_H
#define INSPECTOR_NEW_PROJECT_PRESENTER_H

#include <memory>

#include <QObject>

#include "inspector/ui/NewProject.h"

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

  NewProjectPresenterImp(NewProjectView *view, ProjectModel *model);
  ~NewProjectPresenterImp() override;

// INewProjectPresenter interface

protected slots:

  void saveProject() override;
  void discartProject() override;

// IPresenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;

private:

  NewProjectView *mView;
  ProjectModel *mProjectModel;
  QString mProjectsDefaultPath;
  HelpDialog *mHelp;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_NEW_PROJECT_PRESENTER_H
