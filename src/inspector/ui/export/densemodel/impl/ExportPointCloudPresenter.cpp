#include "ExportPointCloudPresenter.h"

#include "inspector/ui/export/densemodel/impl/ExportPointCloudModel.h"
#include "inspector/ui/export/densemodel/impl/ExportPointCloudView.h"
#include "inspector/ui/HelpDialog.h"
#include "inspector/widgets/PointCloudCSVFormatWidget.h"
//#include "inspector/core/densification/DenseExport.h"

#include <tidop/core/defs.h>
#include <tidop/core/messages.h>

#include <QFileDialog>

namespace inspector
{

namespace ui
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
//  if (mPointCloudCSVFormatWidget){
//    delete mPointCloudCSVFormatWidget;
//    mPointCloudCSVFormatWidget = nullptr;
//  }
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


} // namespace ui

} // namespace inspector
