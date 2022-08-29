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

#include "OrthophotoPresenter.h"

#include "graphos/core/ortho/Orthomosaic.h"
#include "graphos/core/process/Progress.h"
#include "graphos/components/orthophoto/impl/OrthophotoModel.h"
#include "graphos/components/orthophoto/impl/OrthophotoView.h"
#include "graphos/components/orthophoto/impl/OrthophotoProcess.h"
#include "graphos/core/utils.h"

#include <tidop/core/defs.h>

#include <QFileInfo>
#include <QMessageBox>

namespace graphos
{

OrthophotoPresenterImp::OrthophotoPresenterImp(OrthophotoView *view,
                                               OrthophotoModel *model)
  : OrthophotoPresenter(),
    mView(view),
    mModel(model)
{
  this->init();
  this->initSignalAndSlots();
}

OrthophotoPresenterImp::~OrthophotoPresenterImp()
{

}

void OrthophotoPresenterImp::open()
{
  mModel->loadSettings();
  OrthophotoParameters *parameters = mModel->parameters();

  mView->setResolution(parameters->resolution());

  mView->exec();
}

void OrthophotoPresenterImp::init()
{

}

void OrthophotoPresenterImp::initSignalAndSlots()
{
  connect(mView, &ProcessView::run,     this,   &ProcessPresenter::run);
  connect(mView, &DialogView::help, [&]() {
    emit help("orthophoto.html");
});
}

void OrthophotoPresenterImp::onError(tl::TaskErrorEvent *event)
{
  ProcessPresenter::onError(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Orthophoto process error"));
  }
}

void OrthophotoPresenterImp::onFinished(tl::TaskFinalizedEvent *event)
{
  ProcessPresenter::onFinished(event);

  if (progressHandler()) {
    progressHandler()->setDescription(tr("Orthophoto finished"));
  }
}

std::unique_ptr<tl::Task> OrthophotoPresenterImp::createProcess()
{
  std::unique_ptr<tl::Task> ortho_process;

  tl::Path ortho_path = mModel->orthoPath();
  if (!ortho_path.empty()) {
    int i_ret = QMessageBox(QMessageBox::Warning,
                            tr("Previous results"),
                            tr("The previous results will be overwritten. Do you wish to continue?"),
                            QMessageBox::Yes | QMessageBox::No).exec();
    if (i_ret == QMessageBox::No) {
      return ortho_process;
    }
  }

  mModel->clearProject();
  
  OrthophotoParameters *parameters = mModel->parameters();
  parameters->setResolution(mView->resolution());

  std::shared_ptr<OrthophotoAlgorithm> algorithm = std::make_shared<OrthophotoAlgorithm>(mView->resolution(),
                                                                                         mModel->images(),
                                                                                         mModel->cameras(),
                                                                                         mModel->orthoPath(),
                                                                                         mModel->dtmPath(),
                                                                                         mModel->epsCode(),
                                                                                         mModel->useCuda());

  ortho_process = std::make_unique<OrthophotoProcess>(algorithm);
  
  if (progressHandler()){
    progressHandler()->setRange(0, 0);
    progressHandler()->setTitle("Computing Orthophoto...");
    progressHandler()->setDescription("Computing Orthophoto...");
  }
  
  mView->hide();
  
  return ortho_process;
}

void OrthophotoPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}


} // namespace graphos
