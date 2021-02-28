#include "Component.h"

#include "inspector/interfaces/mvp.h"

#include <QAction>

namespace inspector
{

namespace ui
{

ComponentBase::ComponentBase()
  : mAction(nullptr),
    mModel(nullptr),
    mView(nullptr)
{
  this->init();
}

ComponentBase::~ComponentBase()
{
  if (mAction) {
    delete mAction;
    mAction = nullptr;
  }

  if (mModel) {
    delete mModel;
    mModel = nullptr;
  }

  if (mPresenter) {
    delete mPresenter;
    mPresenter = nullptr;
  }
}

void ComponentBase::init()
{
  this->createAction();
  connect(mAction, &QAction::triggered,
          this, &ComponentBase::createComponent);
}

void ComponentBase::createComponent()
{
  disconnect(mAction, &QAction::triggered,
             this, &ComponentBase::createComponent);

  this->createModel();
  this->createView();
  this->createPresenter();

  connect(mAction, &QAction::triggered,
          mPresenter, &IPresenter::open);

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

tl::EnumFlags<Component::Dependencies> ComponentBase::dependencies() const
{
  return mDependencies;
}

//IModel *ComponentBase::model()
//{
//  return mModel;
//}
//
//IDialogView *ComponentBase::view()
//{
//  return mView;
//}

//IPresenter *ComponentBase::presenter()
//{
//  return mPresenter;
//}

void ComponentBase::setName(const QString &name)
{
  mName = name;
  mAction->setText(name);
}

//void ComponentBase::setAction(QAction *action)
//{
//  mAction = action;
//}

void ComponentBase::setMenu(const QString &menu)
{
  mMenu = menu;
}

void ComponentBase::setToolbar(const QString &toolbar)
{
  mToolbar = toolbar;
}

void ComponentBase::setDependencies(const tl::EnumFlags<Dependencies> &dependencies)
{
  mDependencies = dependencies;
}


} // namespace ui

} // namespace inspector
