#include "OpenProjectPresenter.h"

#include "graphos/ui/openproject/OpenProjectModel.h"
#include "graphos/ui/openproject/OpenProjectView.h"
#include "graphos/ui/AppStatus.h"

#include <QFileDialog>
#include <QMessageBox>

namespace graphos
{

namespace ui
{

OpenProjectPresenterImp::OpenProjectPresenterImp(OpenProjectView *view,
                                                 OpenProjectModel *model)
  : OpenProjectPresenter(),
    mView(view),
    mModel(model)
{
  this->init();
  this->initSignalAndSlots();
}

OpenProjectPresenterImp::~OpenProjectPresenterImp()
{
}

void OpenProjectPresenterImp::setProjectFile(const QString &file)
{
  AppStatus &app_status = AppStatus::instance();

  if (!file.isEmpty()) {
    if (app_status.isActive(AppStatus::Flag::project_modified)) {
      int i_ret = QMessageBox(QMessageBox::Information,
                              tr("Save Changes"),
                              tr("There are unsaved changes. Do you want to save them?"),
                              QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel).exec();
      if (i_ret == QMessageBox::Yes) {
        mModel->saveProject();
        app_status.clear();
      } else if (i_ret == QMessageBox::Cancel) {
        return;
      }
    }

    mModel->clear();
    mModel->loadProject(file);

    emit projectLoaded();
  }
}

void OpenProjectPresenterImp::open()
{
  mView->setInspectorPath(mModel->inspectorDirectory());
  mView->exec();
}

void OpenProjectPresenterImp::setHelp(HelpDialog *help)
{
}

void OpenProjectPresenterImp::init()
{
}

void OpenProjectPresenterImp::initSignalAndSlots()
{
  //connect(mView, &OpenProjectView::accepted,      this, &OpenProjectPresenterImp::run);
  //connect(mView, &OpenProjectView::rejected,      this, &OpenProjectPresenterImp::cancel);
  connect(mView, &OpenProjectView::fileSelected, this, &OpenProjectPresenterImp::setProjectFile);
}

void OpenProjectPresenterImp::help()
{
}

} // namespace ui

} // namespace graphos