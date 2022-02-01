/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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

#include "OrthophotoPresenter.h"

#include "graphos/process/orthophoto/OrthophotoProcess.h"
#include "graphos/core/Orthophoto.h"
#include "graphos/core/process/Progress.h"
#include "graphos/components/orthophoto/impl/OrthophotoModel.h"
#include "graphos/components/orthophoto/impl/OrthophotoView.h"
#include "graphos/core/utils.h"
#include "graphos/process/MultiProcess.h"
#include "graphos/components/HelpDialog.h"

#include <tidop/core/defs.h>
#include <tidop/geospatial/photo.h>

//#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

namespace graphos
{

OrthophotoPresenterImp::OrthophotoPresenterImp(OrthophotoView *view,
                                               OrthophotoModel *model)
  : OrthophotoPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

OrthophotoPresenterImp::~OrthophotoPresenterImp()
{

}

void OrthophotoPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("orthophoto.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void OrthophotoPresenterImp::open()
{
  mModel->loadSettings();
  OrthophotoParameters *parameters = mModel->parameters();

  mView->setResolution(parameters->resolution());

  mView->exec();
}

void OrthophotoPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void OrthophotoPresenterImp::init()
{

}

void OrthophotoPresenterImp::initSignalAndSlots()
{
  connect(mView, &ProcessView::run,     this,   &ProcessPresenter::run);
  connect(mView, &DialogView::help,     this,   &Presenter::help);
}

void OrthophotoPresenterImp::onError(int code, const QString &msg)
{
  ProcessPresenter::onError(code, msg);

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Orthophoto process error"));
  }
}

void OrthophotoPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Orthophoto finished"));
  }
}

bool OrthophotoPresenterImp::createProcess()
{
  QString ortho_path = mModel->orthoPath();
  if (!ortho_path.isEmpty()) {
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes | QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      return false;
    }
  }

  mModel->clearProject();

  mMultiProcess->clearProcessList();
  
  OrthophotoParameters *parameters = mModel->parameters();
  parameters->setResolution(mView->resolution());

  std::shared_ptr<OrthophotoAlgorithm> algorithm = std::make_shared<OrthophotoAlgorithm>(mView->resolution(),
                                                                                         mModel->images(),
                                                                                         mModel->cameras(),
                                                                                         mModel->orthoPath(),
                                                                                         mModel->dtmPath(),
                                                                                         mModel->epsCode(),
                                                                                         mModel->useCuda());

  std::shared_ptr<OrthophotoProcess> process = std::make_unique<OrthophotoProcess>(algorithm);
  
  connect(process.get(), SIGNAL(finished()), this, SLOT(onOrthophotoFinished()));
  
  mMultiProcess->appendProcess(process);
  
  if (mProgressHandler){
    mProgressHandler->setRange(0, 0);
    mProgressHandler->setTitle("Computing Orthophoto...");
    mProgressHandler->setDescription("Computing Orthophoto...");
  }
  
  mView->hide();
  
  return true;
}

void OrthophotoPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

void OrthophotoPresenterImp::onOrthophotoFinished()
{

}

} // namespace graphos
