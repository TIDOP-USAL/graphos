#include "FeaturesViewerPresenter.h"

#include <tidop/core/messages.h>

#include "graphos/ui/featviewer/FeaturesViewerModel.h"
#include "graphos/ui/featviewer/FeaturesViewerView.h"
#include "graphos/ui/HelpDialog.h"

#include <QFileInfo>

namespace graphos
{

namespace ui
{

FeaturesViewerPresenterImp::FeaturesViewerPresenterImp(FeaturesViewerView *view,
                                                       FeaturesViewerModel *model)
  : FeaturesViewerPresenter(),
    mView(view),
    mModel(model),
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
  connect(mView, &DialogView::help,               this, &FeaturesViewerPresenterImp::help);
}

void FeaturesViewerPresenterImp::setImageActive(const QString &image)
{
  mView->setCurrentImage(image);
  loadKeypoints(image);
}

void FeaturesViewerPresenterImp::loadKeypoints(const QString &image)
{
  try	{
    std::vector<QPointF> keypoints = mModel->loadKeypoints(image);
    mView->setKeyPoints(keypoints);
  } catch (const std::exception &e) 	{
    msgError(e.what());
  }
}

} // namespace ui

} // namespace graphos
