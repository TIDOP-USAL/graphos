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

#include "ExportPointCloudPresenter.h"

#include "graphos/components/export/densemodel/impl/ExportPointCloudModel.h"
#include "graphos/components/export/densemodel/impl/ExportPointCloudView.h"
#include "graphos/components/HelpDialog.h"
#include "graphos/widgets/PointCloudCSVFormatWidget.h"

#include <tidop/core/defs.h>
#include <tidop/core/messages.h>

#include <QFileDialog>

namespace graphos
{

ExportPointCloudPresenterImp::ExportPointCloudPresenterImp(ExportPointCloudView *view,
                                                           ExportPointCloudModel *model)
  : mView(view),
    mModel(model),
    //mPointCloudCSVFormatWidget(new PointCloudCSVFormatWidgetImp),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

ExportPointCloudPresenterImp::~ExportPointCloudPresenterImp()
{
}

void ExportPointCloudPresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void ExportPointCloudPresenterImp::open()
{
  QString selectedFilter;
  mFile = QFileDialog::getSaveFileName(Q_NULLPTR,
                                       tr("Export Point Cloud"),
                                       "",
                                       "Comma-separated values (*.csv)",
                                       &selectedFilter);
  if (mFile.isEmpty()) return;

  if (selectedFilter.compare("Comma-separated values (*.csv)") == 0){
    mFormat = "CSV";
  } else {
    msgError("Unsupported format");
  }
  //mView->setCurrentFormat(mPointCloudCSVFormatWidget->windowTitle());

  mView->exec();
}

void ExportPointCloudPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void ExportPointCloudPresenterImp::init()
{
//  mView->addFormatWidget(mPointCloudCSVFormatWidget);
//  mView->setCurrentFormat(mPointCloudCSVFormatWidget->windowTitle());
}

void ExportPointCloudPresenterImp::initSignalAndSlots()
{
//  connect(mView, &ExportPointCloudView::previewCSV,                 this, &ExportPointCloudPresenterImp::previewCSV);
//  connect(mView, &DialogView::help,                             this, &ExportPointCloudPresenterImp::help);
//  connect(mView, &ExportPointCloudView::crsInputChanged,            this, &ExportPointCloudPresenterImp::checkInputCRS);
//  connect(mView, &ExportPointCloudView::crsOutputChanged,           this, &ExportPointCloudPresenterImp::checkOutputCRS);

//  connect(mView, &ExportPointCloudView::loadFieldNamesFromFirstRow, mModel, &ExportPointCloudModel::setFieldNamesFromFirstRow);
//  connect(mView, &ExportPointCloudView::delimiterChanged,           mModel, &ExportPointCloudModel::setDelimiter);
//  connect(mView, &ExportPointCloudView::imageFieldIdChanged,        mModel, &ExportPointCloudModel::setImageFieldId);
//  connect(mView, &ExportPointCloudView::xFieldIdChanged,            mModel, &ExportPointCloudModel::setXFieldId);
//  connect(mView, &ExportPointCloudView::yFieldIdChanged,            mModel, &ExportPointCloudModel::setYFieldId);
//  connect(mView, &ExportPointCloudView::zFieldIdChanged,            mModel, &ExportPointCloudModel::setZFieldId);
//  //connect(mView, &ExportPointCloudView::crsChanged,                 mModel, &ExportPointCloudModel::setCRS);
//  connect(mView, &ExportPointCloudView::loadFieldNamesFromFirstRow, this, &ExportPointCloudPresenterImp::previewCSV);
//  connect(mView, &ExportPointCloudView::delimiterChanged,           this, &ExportPointCloudPresenterImp::previewCSV);

//  connect(mView, &ExportPointCloudView::accepted,     mModel, &ExportPointCloudModel::importCameras);

//  connect(mModel, &ExportPointCloudModel::csvHeader,   mView, &ExportPointCloudView::setTableHeader);
//  connect(mModel, &ExportPointCloudModel::imageColumn, mView, &ExportPointCloudView::setImageColumn);
//  connect(mModel, &ExportPointCloudModel::xColumn,     mView, &ExportPointCloudView::setXColumn);
//  connect(mModel, &ExportPointCloudModel::yColumn,     mView, &ExportPointCloudView::setYColumn);
//  connect(mModel, &ExportPointCloudModel::zColumn,     mView, &ExportPointCloudView::setZColumn);
//  connect(mModel, &ExportPointCloudModel::qxColumn,    mView, &ExportPointCloudView::setQxColumn);
//  connect(mModel, &ExportPointCloudModel::qyColumn,    mView, &ExportPointCloudView::setQyColumn);
//  connect(mModel, &ExportPointCloudModel::qzColumn,    mView, &ExportPointCloudView::setQzColumn);
//  connect(mModel, &ExportPointCloudModel::qwColumn,    mView, &ExportPointCloudView::setQwColumn);
//  connect(mModel, &ExportPointCloudModel::omegaColumn, mView, &ExportPointCloudView::setOmegaColumn);
//  connect(mModel, &ExportPointCloudModel::phiColumn,   mView, &ExportPointCloudView::setPhiColumn);
//  connect(mModel, &ExportPointCloudModel::kappaColumn, mView, &ExportPointCloudView::setKappaColumn);
//  connect(mModel, &ExportPointCloudModel::yawColumn,   mView, &ExportPointCloudView::setYawColumn);
//  connect(mModel, &ExportPointCloudModel::pitchColumn, mView, &ExportPointCloudView::setPitchColumn);
//  connect(mModel, &ExportPointCloudModel::rollColumn,  mView, &ExportPointCloudView::setRollColumn);
//  connect(mModel, &ExportPointCloudModel::parseOk,     mView, &ExportPointCloudView::setParseOk);

  connect(mView, &ExportPointCloudView::accepted,     this, &ExportPointCloudPresenter::save);
}

void ExportPointCloudPresenterImp::save()
{
  ///TODO: Comentado porque aqui no esta bien
  //std::array<double, 3> offset = mModel->offset();
  //DenseExport denseExport(mModel->denseModel());
  //denseExport.setOffset(offset[0], offset[1], offset[2]);

  //tl::EnumFlags<DenseExport::Fields> flag(DenseExport::Fields::xyz);
  //if (mView->isColorActive()) flag.flagOn(DenseExport::Fields::rgb);
  //if (mView->isNormalsActive()) flag.flagOn(DenseExport::Fields::normals);

  //if (mFormat.compare("CSV") == 0) {
  //  denseExport.exportToCSV(mFile, flag);
  //}
}

void ExportPointCloudPresenterImp::setCurrentFormat(const QString &format)
{
  //mView->setCurrentFormat(format);
}

} // namespace graphos
