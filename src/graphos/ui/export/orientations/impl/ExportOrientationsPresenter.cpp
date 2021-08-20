#include "ExportOrientationsPresenter.h"

#include "graphos/ui/export/orientations/ExportOrientationsModel.h"
#include "graphos/ui/export/orientations/ExportOrientationsView.h"
#include "graphos/widgets/NvmFormatWidget.h"
#include "graphos/widgets/BundlerFormatWidget.h"
#include "graphos/widgets/MveFormatWidget.h"
#include "graphos/widgets/OriTxtFormatWidget.h"
#include "graphos/ui/HelpDialog.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/defs.h>
#include <tidop/geometry/entities/point.h>

// COLMAP
#include <colmap/base/reconstruction.h>

namespace graphos
{

namespace ui
{

ExportOrientationsPresenterImp::ExportOrientationsPresenterImp(ExportOrientationsView *view,
                                                               ExportOrientationsModel *model)
  : mView(view),
    mModel(model),
    mNvmFormatWidget(new NvmFormatWidgetImp),
    mBundlerFormatWidget(new BundlerFormatWidgetImp),
    mMveFormatWidget(new MveFormatWidgetImp),
    mOriTxtFormatWidget(new OriTxtFormatWidgetImp),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

ExportOrientationsPresenterImp::~ExportOrientationsPresenterImp()
{
  if (mNvmFormatWidget){
    delete mNvmFormatWidget;
    mNvmFormatWidget = nullptr;
  }

  if (mBundlerFormatWidget){
    delete mBundlerFormatWidget;
    mBundlerFormatWidget = nullptr;
  }

  if (mMveFormatWidget){
    delete mMveFormatWidget;
    mMveFormatWidget = nullptr;
  }

  if (mOriTxtFormatWidget){
    delete mOriTxtFormatWidget;
    mOriTxtFormatWidget = nullptr;
  }
}

void ExportOrientationsPresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void ExportOrientationsPresenterImp::open()
{
  mView->setCurrentFormat(mNvmFormatWidget->windowTitle());
  mView->exec();
}

void ExportOrientationsPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void ExportOrientationsPresenterImp::init()
{
  mView->addFormatWidget(mNvmFormatWidget);
  mView->addFormatWidget(mBundlerFormatWidget);
  //mView->addFormatWidget(mMveFormatWidget);
  mView->addFormatWidget(mOriTxtFormatWidget);
  mView->setCurrentFormat(mNvmFormatWidget->windowTitle());
}

void ExportOrientationsPresenterImp::initSignalAndSlots()
{
  connect(mView, &ExportOrientationsView::accepted,     this, &ExportOrientationsPresenterImp::save);
  connect(mView, &ExportOrientationsView::formatChange, this, &ExportOrientationsPresenterImp::setCurrentFormat);
  connect(mView, &DialogView::help,                     this, &ExportOrientationsPresenterImp::help);
}

void ExportOrientationsPresenterImp::save()
{
  QString reconstructionPath = mModel->reconstruction();

  colmap::Reconstruction reconstruction;
//#ifdef _DEBUG
//  //Lectura como texto
//  reconstruction.ReadText(reconstructionPath.toStdString());
//#else
  //Lectura como binario
  reconstruction.ReadBinary(reconstructionPath.toStdString());
//#endif

  tl::Point3D offset = mModel->offset();
  OrientationExport orientationExport(&reconstruction, offset);

  QString oriFormat = mView->format();
  if (oriFormat.compare("NVM") == 0) {
    orientationExport.exportMVE(mNvmFormatWidget->file());
  } else if (oriFormat.compare("Bundler") == 0) {
    orientationExport.exportBundler(mBundlerFormatWidget->file());
  } else if (oriFormat.compare("MVE") == 0) {
    orientationExport.exportMVE(mMveFormatWidget->file());
  } else if (oriFormat.compare("TXT") == 0){
    orientationExport.exportOrientation(mOriTxtFormatWidget->file(), mOriTxtFormatWidget->rotation().compare("Quaternions") == 0);
  }

}

void ExportOrientationsPresenterImp::setCurrentFormat(const QString &format)
{
  mView->setCurrentFormat(format);
}

} // namespace ui

} // namespace graphos
