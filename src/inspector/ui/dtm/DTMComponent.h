#ifndef INSPECTOR_DTM_COMPONENT_H
#define INSPECTOR_DTM_COMPONENT_H

#include "inspector/interfaces/Component.h"


namespace inspector
{

class Project;

namespace ui
{

class DtmModel;
class DtmView;
class DtmPresenter;
class DtmProcess;

class DtmComponent
  : public ProcessComponent
{

  Q_OBJECT

public:

  DtmComponent(Project *project);
  ~DtmComponent() override;

  QAction *openAction() const override;
  QString menu() const override;
  QString toolbar() const override;

private:

  void init();
  void initAction();
  void initComponent();
  void initModel();
  void initView();
  void initPresenter();

private:

  QAction *mAction;
  DtmModel *mModel;
  DtmView *mView;
  DtmPresenter *mPresenter;
  DtmProcess *mProcess;
  Project *mProject;
};

} // namespace ui

} // namespace inspector


#endif // INSPECTOR_DTM_COMPONENT_H
