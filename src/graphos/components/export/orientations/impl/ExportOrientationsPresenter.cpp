/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "ExportOrientationsPresenter.h"

#include "graphos/components/export/orientations/ExportOrientationsModel.h"
#include "graphos/components/export/orientations/ExportOrientationsView.h"
#include "graphos/widgets/NvmFormatWidget.h"
#include "graphos/widgets/BundlerFormatWidget.h"
#include "graphos/widgets/MveFormatWidget.h"
#include "graphos/widgets/OriTxtFormatWidget.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/defs.h>
#include <tidop/geometry/entities/point.h>

// COLMAP
#include <colmap/base/reconstruction.h>

namespace graphos
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

} // namespace graphos
