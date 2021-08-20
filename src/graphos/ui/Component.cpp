#include "Component.h"

#include "graphos/interfaces/mvp.h"
#include "graphos/ui/AppStatus.h"
#include "graphos/ui/process/ProcessPresenter.h"

#include <QAction>

namespace graphos
{

namespace ui
{

ComponentBase::ComponentBase()
  : mAction(nullptr),
    mModel(nullptr),
    mView(nullptr),
    mPresenter(nullptr)
{
  this->init();
}

ComponentBase::~ComponentBase()
{
  if (mAction) {
    delete mAction;
    mAction = nullptr;
  }

  if (mPresenter) {
    delete mPresenter;
    mPresenter = nullptr;
  }

  if (mModel) {
    delete mModel;
    mModel = nullptr;
  }

  if (mView) {
    delete mView;
    mView = nullptr;
  }
}

void ComponentBase::init()
{
  this->createAction();
  connect(mAction, &QAction::triggered,
          this, &ComponentBase::createComponent);

  AppStatus &app_status = AppStatus::instance();
  connect(&app_status, &AppStatus::update,
          this, &ComponentBase::update);
}

void ComponentBase::createComponent()
{
  disconnect(mAction, &QAction::triggered,
             this, &ComponentBase::createComponent);

  this->createModel();
  this->createView();
  this->createPresenter();

  emit created();

  connect(mAction, &QAction::triggered,
          mPresenter, &Presenter::open);

  mPresenter->open();
}

void ComponentBase::createAction()
{
  mAction = new QAction();
}

QString ComponentBase::name() const
{
  return mName;
}

QAction *ComponentBase::action() const
{
  return mAction;
}

QString ComponentBase::menu() const
{
  return mMenu;
}

QString ComponentBase::toolbar() const
{
  return mToolbar;
}

void ComponentBase::setName(const QString &name)
{
  mName = name;
  mAction->setText(name);
}

void ComponentBase::setMenu(const QString &menu)
{
  mMenu = menu;
}

void ComponentBase::setToolbar(const QString &toolbar)
{
  mToolbar = toolbar;
}



ProcessComponent::ProcessComponent()
  : ComponentBase()
{
  connect(this, &Component::created, 
          this, &ProcessComponent::onComponentCreated);

}

void ProcessComponent::setProgressHandler(ProgressHandler *progressHandler)
{
  mProgressHandler = progressHandler;
}

void ProcessComponent::onComponentCreated()
{
  connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::running,
          this, &ProcessComponent::onRunning);
  connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::finished,
          this, &ProcessComponent::onFinished);
  connect(dynamic_cast<ProcessPresenter *>(mPresenter), &ProcessPresenter::failed,
          this, &ProcessComponent::onFailed);

  dynamic_cast<ProcessPresenter *>(mPresenter)->setProgressHandler(mProgressHandler);
}

void ProcessComponent::onRunning()
{
  //emit running(name());
  AppStatus::instance().activeFlag(AppStatus::Flag::processing, true);
}

void ProcessComponent::onFinished()
{
  //emit finished(name());
  AppStatus &appstatus = AppStatus::instance();
  appstatus.activeFlag(AppStatus::Flag::processing, false);
  appstatus.activeFlag(AppStatus::Flag::project_modified, true);
}

void ProcessComponent::onFailed()
{
  AppStatus::instance().activeFlag(AppStatus::Flag::processing, false);
  //emit failed(name());
}

} // namespace ui

} // namespace graphos
