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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/


#include "OrthophotoPresenter.h"

#include "graphos/process/orthophoto/OrthophotoProcess.h"
#include "graphos/core/Orthophoto.h"
#include "graphos/core/process/Progress.h"
#include "graphos/ui/orthophoto/impl/OrthophotoModel.h"
#include "graphos/ui/orthophoto/impl/OrthophotoView.h"
#include "graphos/core/utils.h"

//#include "cvstudio/ui/HelpDialog.h"

#include <tidop/core/defs.h>

#include <QFileDialog>

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
  //if (mHelp){
  //  TL_TODO("Añadir ayuda")
  //  mHelp->setPage("");
  //  mHelp->setModal(true);
  //  mHelp->showMaximized();
  //}
}

void OrthophotoPresenterImp::open()
{
  mModel->loadSettings();
  OrthophotoParameters *parameters = mModel->parameters();

  /* Configure View here */
  
  mView->exec();
}

void OrthophotoPresenterImp::setHelp(HelpDialog *help)
{
  //mHelp = help;
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
    mProgressHandler->setDescription(tr("Process error"));
  }
}

void OrthophotoPresenterImp::onFinished()
{
  ProcessPresenter::onFinished();

  if (mProgressHandler) {
    mProgressHandler->setDescription(tr("Process finished"));
  }
}

bool OrthophotoPresenterImp::createProcess()
{
  
  // std::shared_ptr<ImageProcess> image_process = std::make_shared<OrthophotoAlgorithm>();

  // mProcess = std::make_unique<process::OrthophotoProcess>(this->image(), image_process);

  mView->hide();
  
  return true;
}

void OrthophotoPresenterImp::cancel()
{
  ProcessPresenter::cancel();

  msgWarning("Processing has been canceled by the user");
}

} // namespace graphos
