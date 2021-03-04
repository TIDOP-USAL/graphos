#include "FeatureExtractorComponent.h"

#include "inspector/ui/featextract/impl/FeatureExtractorModel.h"
#include "inspector/ui/featextract/impl/FeatureExtractorView.h"
#include "inspector/ui/featextract/impl/FeatureExtractorPresenter.h"
#include "inspector/core/project.h"
#include "inspector/ui/AppStatus.h"

#include <QAction>
#include <QString>

namespace inspector
{

namespace ui
{


FeatureExtractorComponent::FeatureExtractorComponent(Project *project)
  : ProcessComponent(),
    mProject(project)
{
  this->setName("Feature Extractor");
  this->setMenu("workflow");
  this->setToolbar("workflow");
  QIcon iconFeatureExtraction;
  iconFeatureExtraction.addFile(QStringLiteral(":/ico/24/img/material/24/features.png"), QSize(), QIcon::Normal, QIcon::Off);
  mAction->setIcon(iconFeatureExtraction);
}

FeatureExtractorComponent::~FeatureExtractorComponent()
{
}

void FeatureExtractorComponent::createModel()
{
  mModel = new FeatureExtractorModelImp(mProject);
}

void FeatureExtractorComponent::createView()
{
  mView = new FeatureExtractorViewImp();
}

void FeatureExtractorComponent::createPresenter()
{
  mPresenter = new FeatureExtractorPresenterImp(dynamic_cast<FeatureExtractorView *>(mView), 
                                                dynamic_cast<FeatureExtractorModel *>(mModel));
}

void FeatureExtractorComponent::update()
{
  AppStatus &app_status = AppStatus::instance();
  bool bProjectExists = app_status.isActive(AppStatus::Flag::project_exists);
  bool bProcessing = app_status.isActive(AppStatus::Flag::processing);
  bool bImagesLoaded = app_status.isActive(AppStatus::Flag::images_added);
  mAction->setEnabled(bProjectExists && bImagesLoaded && !bProcessing);
}

void FeatureExtractorComponent::onRunning()
{
  ProcessComponent::onRunning();
}

void FeatureExtractorComponent::onFinished()
{
  ProcessComponent::onFinished();
  AppStatus::instance().activeFlag(AppStatus::Flag::feature_extraction, true);
}

void FeatureExtractorComponent::onFailed()
{
  ProcessComponent::onFailed();
  AppStatus::instance().activeFlag(AppStatus::Flag::feature_extraction, false);
}


} // namespace ui

} // namespace inspector
