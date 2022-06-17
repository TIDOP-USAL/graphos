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

#include "ImportCamerasPresenter.h"

#include "graphos/components/import/cameras/impl/ImportCamerasModel.h"
#include "graphos/components/import/cameras/impl/ImportCamerasView.h"

#include <tidop/core/defs.h>

#include <QFileDialog>

namespace graphos
{

ImportCamerasPresenterImp::ImportCamerasPresenterImp(ImportCamerasView *view,
                                                     ImportCamerasModel *model)
  : mView(view),
    mModel(model)
{
  this->init();
  this->initSignalAndSlots();
}

ImportCamerasPresenterImp::~ImportCamerasPresenterImp()
{
}

void ImportCamerasPresenterImp::previewCSV()
{
  //mModel->clear();
  mModel->previewImportCameras();
}

//bool ImportCamerasPresenterImp::checkCRS(const QString &crs)
//{
//  tl::geospatial::Crs _crs(crs.toStdString());
//  return _crs.isValid();
//}

void ImportCamerasPresenterImp::checkInputCRS(const QString &crs)
{
  mView->setValidInputCRS(mModel->checkCRS(crs));
  mModel->setInputCRS(crs);
}

void ImportCamerasPresenterImp::checkOutputCRS(const QString &crs)
{
  mView->setValidOutputCRS(mModel->checkCRS(crs));
  mModel->setOutputCRS(crs);
}

void ImportCamerasPresenterImp::open()
{
  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
                                              tr("Orientation cameras file"),
                                              "",
                                              tr("Comma-separated values (*.csv);;Plain text (*.txt)"));
  if (!file.isEmpty()){
    mModel->setCsvFile(file);
    mView->setOutputCRS(mModel->outputCRS());
    previewCSV();
  } else {
    return;
  }

  mView->exec();
}

void ImportCamerasPresenterImp::init()
{
  mView->setItemModel(mModel->itemModelCSV());
  mView->setItemModelFormatCameras(mModel->itemModelFormatCameras());
}

void ImportCamerasPresenterImp::initSignalAndSlots()
{
  connect(mView, &ImportCamerasView::previewCSV,                 this, &ImportCamerasPresenterImp::previewCSV);
  connect(mView, &ImportCamerasView::crsInputChanged,            this, &ImportCamerasPresenterImp::checkInputCRS);
  connect(mView, &ImportCamerasView::crsOutputChanged,           this, &ImportCamerasPresenterImp::checkOutputCRS);

  connect(mView, &ImportCamerasView::loadFieldNamesFromFirstRow, mModel, &ImportCamerasModel::setFieldNamesFromFirstRow);
  connect(mView, &ImportCamerasView::skipLines,                  mModel, &ImportCamerasModel::setInitialLine);
  connect(mView, &ImportCamerasView::delimiterChanged,           mModel, &ImportCamerasModel::setDelimiter);
  connect(mView, &ImportCamerasView::imageFieldIdChanged,        mModel, &ImportCamerasModel::setImageFieldId);
  connect(mView, &ImportCamerasView::xFieldIdChanged,            mModel, &ImportCamerasModel::setXFieldId);
  connect(mView, &ImportCamerasView::yFieldIdChanged,            mModel, &ImportCamerasModel::setYFieldId);
  connect(mView, &ImportCamerasView::zFieldIdChanged,            mModel, &ImportCamerasModel::setZFieldId);
  connect(mView, &ImportCamerasView::rotationChange,             mModel, &ImportCamerasModel::setRotationType);
  connect(mView, &ImportCamerasView::qxFieldChange,            mModel, &ImportCamerasModel::setQxFieldId);
  connect(mView, &ImportCamerasView::qyFieldChange,            mModel, &ImportCamerasModel::setQyFieldId);
  connect(mView, &ImportCamerasView::qzFieldChange,            mModel, &ImportCamerasModel::setQzFieldId);
  connect(mView, &ImportCamerasView::qwFieldChange,            mModel, &ImportCamerasModel::setQwFieldId);
  connect(mView, &ImportCamerasView::pitchFieldChange,            mModel, &ImportCamerasModel::setPitchFieldId);
  connect(mView, &ImportCamerasView::rollFieldChange,            mModel, &ImportCamerasModel::setRollFieldId);
  connect(mView, &ImportCamerasView::yawFieldChange,            mModel, &ImportCamerasModel::setYawFieldId);
  connect(mView, &ImportCamerasView::omegaFieldChange,            mModel, &ImportCamerasModel::setOmegaFieldId);
  connect(mView, &ImportCamerasView::phiFieldChange,            mModel, &ImportCamerasModel::setPhiFieldId);
  connect(mView, &ImportCamerasView::kappaFieldChange,            mModel, &ImportCamerasModel::setKappaFieldId);
  //connect(mView, &ImportCamerasView::crsChanged,                 mModel, &ImportCamerasModel::setCRS);
  connect(mView, &ImportCamerasView::loadFieldNamesFromFirstRow, this, &ImportCamerasPresenterImp::previewCSV);
  connect(mView, &ImportCamerasView::delimiterChanged,           this, &ImportCamerasPresenterImp::previewCSV);
  connect(mView, &ImportCamerasView::skipLines,                  this, &ImportCamerasPresenterImp::previewCSV);

  connect(mModel, &ImportCamerasModel::csvHeader,   mView, &ImportCamerasView::setTableHeader);
  connect(mModel, &ImportCamerasModel::imageColumn, mView, &ImportCamerasView::setImageColumn);
  connect(mModel, &ImportCamerasModel::xColumn,     mView, &ImportCamerasView::setXColumn);
  connect(mModel, &ImportCamerasModel::yColumn,     mView, &ImportCamerasView::setYColumn);
  connect(mModel, &ImportCamerasModel::zColumn,     mView, &ImportCamerasView::setZColumn);
  connect(mModel, &ImportCamerasModel::qxColumn,    mView, &ImportCamerasView::setQxColumn);
  connect(mModel, &ImportCamerasModel::qyColumn,    mView, &ImportCamerasView::setQyColumn);
  connect(mModel, &ImportCamerasModel::qzColumn,    mView, &ImportCamerasView::setQzColumn);
  connect(mModel, &ImportCamerasModel::qwColumn,    mView, &ImportCamerasView::setQwColumn);
  connect(mModel, &ImportCamerasModel::omegaColumn, mView, &ImportCamerasView::setOmegaColumn);
  connect(mModel, &ImportCamerasModel::phiColumn,   mView, &ImportCamerasView::setPhiColumn);
  connect(mModel, &ImportCamerasModel::kappaColumn, mView, &ImportCamerasView::setKappaColumn);
  connect(mModel, &ImportCamerasModel::yawColumn,   mView, &ImportCamerasView::setYawColumn);
  connect(mModel, &ImportCamerasModel::pitchColumn, mView, &ImportCamerasView::setPitchColumn);
  connect(mModel, &ImportCamerasModel::rollColumn,  mView, &ImportCamerasView::setRollColumn);
  connect(mModel, &ImportCamerasModel::parseOk,     mView, &ImportCamerasView::setParseOk);

  connect(mView, &ImportCamerasView::accepted,     this, &ImportCamerasPresenter::importedCameras);

  connect(mView, &ImportCamerasView::accepted, mModel, &ImportCamerasModel::importCameras);
  connect(mView, &DialogView::help, [&]() {
    emit help("cameras.html");
  });
}

} // namespace graphos
