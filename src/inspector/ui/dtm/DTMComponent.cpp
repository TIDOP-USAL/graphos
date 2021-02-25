#include "DTMComponent.h"

#include "inspector/ui/dtm/impl/DTMModel.h"
#include "inspector/ui/dtm/impl/DTMView.h"
#include "inspector/ui/dtm/impl/DTMPresenter.h"
#include "inspector/core/project.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{

DtmComponent::DtmComponent(Project *project)
  : ProcessComponent(),
    mAction(nullptr),
    mModel(nullptr),
    mView(nullptr),
    mPresenter(nullptr),
    mProject(project)
{
  init();
}

DtmComponent::~DtmComponent()
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

QAction *DtmComponent::openAction() const
{
  return mAction;
}

QString DtmComponent::menu() const
{
  return "tools";
}

QString DtmComponent::toolbar() const
{
  return QString();
}

void DtmComponent::init()
{
  initAction();
  connect(mAction, &QAction::triggered,
          this, &DtmComponent::initComponent);
}

void DtmComponent::initAction()
{
  mAction = new QAction();
  mAction->setText(tr("DTM/DSM"));
  //QIcon iconOpenFile;
  //iconOpenFile.addFile(QStringLiteral(":/ico/24px/icons/material/24px/icons8_opened_folder_24px_2.png"), QSize(), QIcon::Normal, QIcon::Off);
  //mAction->setIcon(iconOpenFile);
}

void DtmComponent::initComponent()
{
  disconnect(mAction, &QAction::triggered,
             this, &DtmComponent::initComponent);

  this->initModel();
  this->initView();
  this->initPresenter();

  connect(mAction, &QAction::triggered,
          this, &DtmComponent::initComponent);

  mPresenter->open();
}

void DtmComponent::initModel()
{
  mModel = new DtmModelImp(mProject);
}

void DtmComponent::initView()
{
  mView = new DtmViewImp();
}

void DtmComponent::initPresenter()
{
  mPresenter = new DtmPresenterImp(mView, mModel);

  connect(mPresenter, &ProcessPresenter::running,
          this, &ProcessComponent::running);
  connect(mPresenter, &ProcessPresenter::finished,
          this, &ProcessComponent::finished);
  connect(mPresenter, &ProcessPresenter::failed,
          this, &ProcessComponent::failed);

}



} // namespace ui

} // namespace inspector
