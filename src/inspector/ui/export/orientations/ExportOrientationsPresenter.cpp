#include "ExportOrientationsPresenter.h"

#include "inspector/ui/export/orientations/ExportOrientationsModel.h"
#include "inspector/ui/export/orientations/ExportOrientationsView.h"
#include "inspector/widgets/NvmFormatWidget.h"
#include "inspector/widgets/BundlerFormatWidget.h"
#include "inspector/ui/HelpDialog.h"

#include <tidop/core/defs.h>

namespace inspector
{

namespace ui
{

ExportOrientationsPresenterImp::ExportOrientationsPresenterImp(ExportOrientationsView *view,
                                                               ExportOrientationsModel *model)
  : mView(view),
    mModel(model),
    mNvmFormatWidget(new NvmFormatWidgetImp),
    mBundlerFormatWidget(new BundlerFormatWidgetImp),
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
  mView->setCurrentFormat(mNvmFormatWidget->windowTitle());
}

void ExportOrientationsPresenterImp::initSignalAndSlots()
{
  connect(mView, &ExportOrientationsView::formatChange, this, &ExportOrientationsPresenterImp::setCurrentFormat);
  connect(mView, &IDialogView::help,                    this, &ExportOrientationsPresenterImp::help);

}

void ExportOrientationsPresenterImp::save()
{
}

void ExportOrientationsPresenterImp::setCurrentFormat(const QString &format)
{
  mView->setCurrentFormat(format);
}

} // namespace ui

} // namespace inspector
