#include "ImportCamerasPresenter.h"

#include "graphos/ui/import/cameras/impl/ImportCamerasModel.h"
#include "graphos/ui/import/cameras/impl/ImportCamerasView.h"
#include "graphos/ui/HelpDialog.h"

#include <tidop/core/defs.h>

#include <QFileDialog>

namespace graphos
{

namespace ui
{

ImportCamerasPresenterImp::ImportCamerasPresenterImp(ImportCamerasView *view,
                                                     ImportCamerasModel *model)
  : mView(view),
    mModel(model),
    mHelp(nullptr)
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

void ImportCamerasPresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
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

void ImportCamerasPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void ImportCamerasPresenterImp::init()
{
  mView->setItemModel(mModel->itemModelCSV());
  mView->setItemModelFormatCameras(mModel->itemModelFormatCameras());
}

void ImportCamerasPresenterImp::initSignalAndSlots()
{
  connect(mView, &ImportCamerasView::previewCSV,                 this, &ImportCamerasPresenterImp::previewCSV);
  connect(mView, &DialogView::help,                             this, &ImportCamerasPresenterImp::help);
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

  connect(mView, &ImportCamerasView::accepted,     mModel, &ImportCamerasModel::importCameras);

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
}


} // namespace ui

} // namespace graphos
