#ifndef INSPECTOR_COMPONENTS_MANAGER_H
#define INSPECTOR_COMPONENTS_MANAGER_H

#include <QObject>

namespace inspector
{

class ProjectController;
class Project;

namespace ui
{
	
class MainWindowModel;
class MainWindowView;
class MainWindowPresenter;

class ProjectModel;

class NewProjectPresenter;


class ComponentsManager : public QObject
{
  Q_OBJECT

public:

  explicit ComponentsManager(QObject *parent = nullptr);
  ~ComponentsManager();


  MainWindowView *mainWindowView();
  MainWindowModel *mainWindowModel();
  MainWindowPresenter *mainWindowPresenter();

  ProjectModel *projectModel();
  
  NewProjectPresenter *newProjectPresenter();
  
signals:

protected slots:

  void initAndOpenNewProjectDialog();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;

  Project *mProject;
  ProjectController *mProjectIO;
  ProjectModel *mProjectModel;

  NewProjectPresenter *mNewProjectPresenter;
  
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENTS_MANAGER_H
