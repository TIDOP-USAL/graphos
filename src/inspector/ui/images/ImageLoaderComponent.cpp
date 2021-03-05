#include "ImageLoaderComponent.h"

#include "inspector/ui/images/impl/ImageLoaderModel.h"
#include "inspector/ui/images/impl/ImageLoaderView.h"
#include "inspector/ui/images/impl/ImageLoaderPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


ImageLoaderComponent::ImageLoaderComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Load Images");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  QIcon iconLoadImages;
  iconLoadImages.addFile(QStringLiteral(":/ico/24/img/material/24/icons8-add-folder.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconLoadImages);
}

ImageLoaderComponent::~ImageLoaderComponent()
{
}

void ImageLoaderComponent::createModel()
{
  mModel = new ImageLoaderModelImp(mProject);
}

void ImageLoaderComponent::createView()
{
  mView = new ImageLoaderViewImp();
}

void ImageLoaderComponent::createPresenter()
{
  mPresenter = new ImageLoaderPresenterImp(dynamic_cast<ImageLoaderView *>(mView), 
                                           dynamic_cast<ImageLoaderModel *>(mModel));

  connect(dynamic_cast<ImageLoaderPresenter *>(mPresenter), &ImageLoaderPresenter::imageLoaded, 
          this, &ImageLoaderComponent::imageLoaded);
}

void ImageLoaderComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bLoadingImages = app_status.isActive(AppStatus::Flag::loading_images);
  mAction->setEnabled(bProjectExists && !bLoadingImages && !bProcessing);
}

void ImageLoaderComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void ImageLoaderComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::loading_images, false);
}

void ImageLoaderComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::loading_images, false);
}


} // namespace ui

} // namespace inspector
