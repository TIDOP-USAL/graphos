#include "GeoreferencePresenter.h"

#include "inspector/ui/georeference/impl/GeoreferenceModel.h"
#include "inspector/ui/georeference/impl/GeoreferenceView.h"
#include "inspector/ui/HelpDialog.h"

#include <tidop/core/defs.h>

namespace inspector
{

namespace ui
{

GeoreferencePresenterImp::GeoreferencePresenterImp(GeoreferenceView *view,
                                                   GeoreferenceModel *model)
  : mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

GeoreferencePresenterImp::~GeoreferencePresenterImp()
{
}
void GeoreferencePresenterImp::help()
{
  if (mHelp){
    TL_TODO("Añadir ayuda")
    mHelp->setPage("");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void GeoreferencePresenterImp::open()
{

  mView->exec();
}

void GeoreferencePresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void GeoreferencePresenterImp::init()
{
  mView->setItemModel(mModel->itemModel());
}

void GeoreferencePresenterImp::initSignalAndSlots()
{
//  connect(mView, &ExportOrientationsView::accepted,     this, &ExportOrientationsPresenterImp::save);
  connect(mView, &GeoreferenceView::loadCSV, mModel, &GeoreferenceModel::importPositionsFromCSV);
  connect(mView, &IDialogView::help,                    this, &GeoreferencePresenterImp::help);

  connect(mModel, &GeoreferenceModel::csvHeader, mView, &GeoreferenceView::setTableHeader);
  connect(mModel, &GeoreferenceModel::imageColumn, mView, &GeoreferenceView::setImageColumn);
  connect(mModel, &GeoreferenceModel::xColumn, mView, &GeoreferenceView::setXColumn);
  connect(mModel, &GeoreferenceModel::yColumn, mView, &GeoreferenceView::setYColumn);
  connect(mModel, &GeoreferenceModel::zColumn, mView, &GeoreferenceView::setZColumn);
  connect(mModel, &GeoreferenceModel::qxColumn, mView, &GeoreferenceView::setQxColumn);
  connect(mModel, &GeoreferenceModel::qyColumn, mView, &GeoreferenceView::setQyColumn);
  connect(mModel, &GeoreferenceModel::qzColumn, mView, &GeoreferenceView::setQzColumn);
  connect(mModel, &GeoreferenceModel::qwColumn, mView, &GeoreferenceView::setQwColumn);

}

} // namespace ui

} // namespace inspector
