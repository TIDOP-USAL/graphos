#include "CameraPositionsPresenter.h"

#include "inspector/ui/import/cameras/impl/CameraPositionsModel.h"
#include "inspector/ui/import/cameras/impl/CameraPositionsView.h"
#include "inspector/ui/HelpDialog.h"

#include <tidop/core/defs.h>

#include <QFileDialog>

namespace inspector
{

namespace ui
{

CamerasImportPresenterImp::CamerasImportPresenterImp(CamerasImportView *view,
                                                     CamerasImportModel *model)
  : mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

CamerasImportPresenterImp::~CamerasImportPresenterImp()
{
}

void CamerasImportPresenterImp::previewCSV()
{
  //mModel->clear();
  mModel->previewImportCameras();
}

//bool CamerasImportPresenterImp::checkCRS(const QString &crs)
//{
//  tl::geospatial::Crs _crs(crs.toStdString());
//  return _crs.isValid();
//}

void CamerasImportPresenterImp::checkInputCRS(const QString &crs)
{
  mView->setValidInputCRS(mModel->checkCRS(crs));
  mModel->setInputCRS(crs);
}

void CamerasImportPresenterImp::checkOutputCRS(const QString &crs)
{
  mView->setValidOutputCRS(mModel->checkCRS(crs));
  mModel->setOutputCRS(crs);
}

void CamerasImportPresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void CamerasImportPresenterImp::open()
{
  QString file = QFileDialog::getOpenFileName(Q_NULLPTR,
                                              tr("Orientation cameras file"),
                                              "",
                                              tr("Comma-separated values (*.csv)"));
  if (!file.isEmpty()){
    //mFile = file;
    mModel->setCsvFile(file);
    previewCSV();
  } else {
    return;
  }

  mView->exec();
}

void CamerasImportPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void CamerasImportPresenterImp::init()
{
  mView->setItemModel(mModel->itemModelCSV());
  mView->setItemModelFormatCameras(mModel->itemModelFormatCameras());
}

void CamerasImportPresenterImp::initSignalAndSlots()
{
  connect(mView, &CamerasImportView::previewCSV,                 this, &CamerasImportPresenterImp::previewCSV);
  connect(mView, &IDialogView::help,                             this, &CamerasImportPresenterImp::help);
  connect(mView, &CamerasImportView::crsInputChanged,            this, &CamerasImportPresenterImp::checkInputCRS);
  connect(mView, &CamerasImportView::crsOutputChanged,           this, &CamerasImportPresenterImp::checkOutputCRS);

  connect(mView, &CamerasImportView::loadFieldNamesFromFirstRow, mModel, &CamerasImportModel::setFieldNamesFromFirstRow);
  connect(mView, &CamerasImportView::delimiterChanged,           mModel, &CamerasImportModel::setDelimiter);
  connect(mView, &CamerasImportView::imageFieldIdChanged,        mModel, &CamerasImportModel::setImageFieldId);
  connect(mView, &CamerasImportView::xFieldIdChanged,            mModel, &CamerasImportModel::setXFieldId);
  connect(mView, &CamerasImportView::yFieldIdChanged,            mModel, &CamerasImportModel::setYFieldId);
  connect(mView, &CamerasImportView::zFieldIdChanged,            mModel, &CamerasImportModel::setZFieldId);
  connect(mView, &CamerasImportView::rotationChange,             mModel, &CamerasImportModel::setRotationType);
  connect(mView, &CamerasImportView::qxFieldChange,            mModel, &CamerasImportModel::setQxFieldId);
  connect(mView, &CamerasImportView::qyFieldChange,            mModel, &CamerasImportModel::setQyFieldId);
  connect(mView, &CamerasImportView::qzFieldChange,            mModel, &CamerasImportModel::setQzFieldId);
  connect(mView, &CamerasImportView::qwFieldChange,            mModel, &CamerasImportModel::setQwFieldId);
  connect(mView, &CamerasImportView::pitchFieldChange,            mModel, &CamerasImportModel::setPitchFieldId);
  connect(mView, &CamerasImportView::rollFieldChange,            mModel, &CamerasImportModel::setRollFieldId);
  connect(mView, &CamerasImportView::yawFieldChange,            mModel, &CamerasImportModel::setYawFieldId);
  connect(mView, &CamerasImportView::omegaFieldChange,            mModel, &CamerasImportModel::setOmegaFieldId);
  connect(mView, &CamerasImportView::phiFieldChange,            mModel, &CamerasImportModel::setPhiFieldId);
  connect(mView, &CamerasImportView::kappaFieldChange,            mModel, &CamerasImportModel::setKappaFieldId);
  //connect(mView, &CamerasImportView::crsChanged,                 mModel, &CamerasImportModel::setCRS);
  connect(mView, &CamerasImportView::loadFieldNamesFromFirstRow, this, &CamerasImportPresenterImp::previewCSV);
  connect(mView, &CamerasImportView::delimiterChanged,           this, &CamerasImportPresenterImp::previewCSV);

  connect(mView, &CamerasImportView::accepted,     mModel, &CamerasImportModel::importCameras);

  connect(mModel, &CamerasImportModel::csvHeader,   mView, &CamerasImportView::setTableHeader);
  connect(mModel, &CamerasImportModel::imageColumn, mView, &CamerasImportView::setImageColumn);
  connect(mModel, &CamerasImportModel::xColumn,     mView, &CamerasImportView::setXColumn);
  connect(mModel, &CamerasImportModel::yColumn,     mView, &CamerasImportView::setYColumn);
  connect(mModel, &CamerasImportModel::zColumn,     mView, &CamerasImportView::setZColumn);
  connect(mModel, &CamerasImportModel::qxColumn,    mView, &CamerasImportView::setQxColumn);
  connect(mModel, &CamerasImportModel::qyColumn,    mView, &CamerasImportView::setQyColumn);
  connect(mModel, &CamerasImportModel::qzColumn,    mView, &CamerasImportView::setQzColumn);
  connect(mModel, &CamerasImportModel::qwColumn,    mView, &CamerasImportView::setQwColumn);
  connect(mModel, &CamerasImportModel::omegaColumn, mView, &CamerasImportView::setOmegaColumn);
  connect(mModel, &CamerasImportModel::phiColumn,   mView, &CamerasImportView::setPhiColumn);
  connect(mModel, &CamerasImportModel::kappaColumn, mView, &CamerasImportView::setKappaColumn);
  connect(mModel, &CamerasImportModel::yawColumn,   mView, &CamerasImportView::setYawColumn);
  connect(mModel, &CamerasImportModel::pitchColumn, mView, &CamerasImportView::setPitchColumn);
  connect(mModel, &CamerasImportModel::rollColumn,  mView, &CamerasImportView::setRollColumn);
  connect(mModel, &CamerasImportModel::parseOk,     mView, &CamerasImportView::setParseOk);

  connect(mView, &CamerasImportView::accepted,     this, &CamerasImportPresenter::projectModified);
}


} // namespace ui

} // namespace inspector
