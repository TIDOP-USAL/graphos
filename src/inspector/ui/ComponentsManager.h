#ifndef INSPECTOR_COMPONENTS_MANAGER_H
#define INSPECTOR_COMPONENTS_MANAGER_H

#include <QObject>

namespace inspector
{

namespace ui
{
	
class MainWindowModel;
class MainWindowView;
class MainWindowPresenter;


class ComponentsManager : public QObject
{
  Q_OBJECT

public:

  explicit ComponentsManager(QObject *parent = nullptr);
  ~ComponentsManager();


  MainWindowView *mainWindowView();
  MainWindowModel *mainWindowModel();
  MainWindowPresenter *mainWindowPresenter();

signals:

protected slots:

  void initAndOpenNewProjectDialog();

protected:

  MainWindowView *mMainWindowView;
  MainWindowModel *mMainWindowModel;
  MainWindowPresenter *mMainWindowPresenter;

};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_COMPONENTS_MANAGER_H
