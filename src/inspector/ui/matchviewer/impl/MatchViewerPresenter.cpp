#include "MatchViewerPresenter.h"

#include "inspector/ui/matchviewer/impl/MatchViewerModel.h"
#include "inspector/ui/matchviewer/impl/MatchViewerView.h"
//#include "inspector/ui/SettingsModel.h"
#include "inspector/ui/HelpDialog.h"

#include <QStandardPaths>
#include <QDir>

namespace inspector
{

namespace ui
{

MatchViewerPresenterImp::MatchViewerPresenterImp(MatchViewerView *view,
                                                 MatchViewerModel *model/*,
                                                 SettingsModel *settings*/)
  : MatchViewerPresenter(),
    mView(view),
    mModel(model),
    //mSettingsModel(settings),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

MatchViewerPresenterImp::~MatchViewerPresenterImp()
{

}

//void MatchViewerPresenterImp::openFromImages(const QString &imageLeft, 
//                                             const QString &imageRight)
//{
//  this->open();
//  this->setLeftImage(imageLeft);
//  this->setRightImage(imageRight);
//}

void MatchViewerPresenterImp::setLeftImage(const QString &image)
{
  mView->setLeftImage(image);
  std::vector<QString> imagesRight = mModel->imagePairs(image);
  if (imagesRight.empty() == false){
    mView->setRightImageList(imagesRight);
    mView->setRightImage(QFileInfo(imagesRight[0]).baseName());
    loadMatches(image, QFileInfo(imagesRight[0]).baseName());
  }
}

void MatchViewerPresenterImp::setRightImage(const QString &image)
{
  mView->setRightImage(image);
}

void MatchViewerPresenterImp::loadMatches(const QString &imageLeft, const QString &imageRight)
{
  std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> matches = mModel->loadMatches(imageLeft, imageRight);
  mView->setMatches(matches);
}

//void MatchViewerPresenterImp::deleteMatch(const QString &imageLeft, const QString &imageRight, int query_id, int train_id)
//{
//  mModel->deleteMatch(imageLeft, imageRight, query_id, train_id);
//}

void MatchViewerPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("matches_viewer.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void MatchViewerPresenterImp::open()
{
  mView->clear();

  mView->setBGColor(mModel->viewerBGColor());
  mView->setSelectedMarkerStyle(mModel->viewerSelectMarkerColor(),
                                mModel->viewerSelectMarkerWidth());
  mView->setMarkerStyle(mModel->viewerMarkerColor(),
                        mModel->viewerMarkerWidth(),
                        mModel->viewerMarkerType(),
                        mModel->viewerMarkerSize());
  mView->setLineStyle(mModel->viewerLineColor(),
                      mModel->viewerLineWidth());

/// Se carga el fichero de puntos de paso.
//  mModel->loadPassPoints();

  mView->show();

  /// Se cargan las imágenes despues de mostrar el Dialog porque si se hace antes
  /// el QGraphicView no tiene el tamaño adecuado
  std::vector<QString> imagesLeft = mModel->images();
  if (imagesLeft.empty() == false) {
    mView->setLeftImageList(imagesLeft);
    setLeftImage(QFileInfo(imagesLeft[0]).baseName());
  }

}

void MatchViewerPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void MatchViewerPresenterImp::init()
{
}

void MatchViewerPresenterImp::initSignalAndSlots()
{
  connect(mView, SIGNAL(leftImageChange(QString)),         this, SLOT(setLeftImage(QString)));
  connect(mView, SIGNAL(rightImageChange(QString)),        this, SLOT(setRightImage(QString)));
  connect(mView, SIGNAL(loadMatches(QString, QString)),    this, SLOT(loadMatches(QString, QString)));

//  connect(mView, SIGNAL(deleteMatch(QString, QString, int, int)),
//          this,  SLOT(deleteMatch(QString, QString, int, int)));

  connect(mView, SIGNAL(help()),     this, SLOT(help()));
}

} // namespace ui

} // namespace inspector
