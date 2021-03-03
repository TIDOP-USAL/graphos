#include "FeaturesViewerPresenter.h"

#include "inspector/ui/featviewer/FeaturesViewerModel.h"
#include "inspector/ui/featviewer/FeaturesViewerView.h"
//#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/HelpDialog.h"

#include <QFileInfo>

namespace inspector
{

namespace ui
{

FeaturesViewerPresenterImp::FeaturesViewerPresenterImp(FeaturesViewerView *view,
                                                       FeaturesViewerModel *model/*,
                                                       SettingsModel *settingsModel*/)
  : FeaturesViewerPresenter(),
    mView(view),
    mModel(model),
    //mSettingsModel(settingsModel),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

FeaturesViewerPresenterImp::~FeaturesViewerPresenterImp()
{

}

void FeaturesViewerPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("keypoints_viewer.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void FeaturesViewerPresenterImp::open()
{
  mView->clear();

  mView->setBGColor(mModel->viewerBGColor());
  mView->setSelectedMarkerStyle(mModel->viewerSelectMarkerColor(),
                                mModel->viewerSelectMarkerWidth());
  mView->setMarkerStyle(mModel->viewerMarkerColor(),
                        mModel->viewerMarkerWidth(),
                        mModel->viewerMarkerType(),
                        mModel->viewerMarkerSize());

  mView->show();

  std::vector<QString> images = mModel->images();
  if (images.empty() == false) {
    mView->setImageList(images);
    setImageActive(QFileInfo(images[0]).baseName());
  }
}

void FeaturesViewerPresenterImp::openKeypointsFromImage(const QString &image)
{
  this->open();
  this->setImageActive(image);
}

void FeaturesViewerPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void FeaturesViewerPresenterImp::init()
{
}

void FeaturesViewerPresenterImp::initSignalAndSlots()
{
  connect(mView, &FeaturesViewerView::imageChange, this, &FeaturesViewerPresenterImp::setImageActive);
  connect(mView, &IDialogView::help,               this, &FeaturesViewerPresenterImp::help);
}

void FeaturesViewerPresenterImp::setImageActive(const QString &image)
{
  mView->setCurrentImage(image);
  loadKeypoints(image);
}

void FeaturesViewerPresenterImp::loadKeypoints(const QString &image)
{
  std::vector<QPointF> keypoints = mModel->loadKeypoints(image);
  mView->setKeyPoints(keypoints);
}

} // namespace ui

} // namespace inspector
