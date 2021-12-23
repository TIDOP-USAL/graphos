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

#include <QDebug>
#include <QFileDialog>
#include <QFile>

#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"


namespace graphos
{

ProgressDialogImp::ProgressDialogImp(QWidget *parent)
  : ProgressDialog(parent),
    ui(new Ui::ProgressDialog),
    mAutoClose(false)
{
  this->initUI();
  this->initSignalAndSlots();
}

ProgressDialogImp::~ProgressDialogImp()
{
  if (ui) {
    delete ui;
    ui = nullptr;
  }
}

void ProgressDialogImp::initUI()
{
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
  this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);
  this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);

  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/ico/app/img/FMELogo.ico"));

  ui->pushButtonMinimize->setEnabled(false);
}

void ProgressDialogImp::initSignalAndSlots()
{
  connect(ui->pushButtonMinimize, SIGNAL(clicked(bool)), this, SLOT(onMinimized()));
  connect(ui->pushButtonCancel, SIGNAL(clicked(bool)), this, SLOT(onPushButtonCancelClicked()));
  connect(ui->pushButtonClose, SIGNAL(clicked(bool)), this, SLOT(close()));
}

void ProgressDialogImp::retranslate()
{
}

void ProgressDialogImp::clear()
{
}

void ProgressDialogImp::update()
{
}

void ProgressDialogImp::setStatusText(QString text)
{
  ui->labelStatus->setText(text);
}

void ProgressDialogImp::setCloseAuto(bool active)
{
  mAutoClose = active;
}

void ProgressDialogImp::setRange(int min, int max)
{
  ui->progressBar->setRange(min, max);
}

void ProgressDialogImp::setValue(int value)
{
  ui->progressBar->setValue(value);
}

void ProgressDialogImp::setInitialized()
{
  //ui->pushButton->setText(tr("Cancel"));
  ui->pushButtonCancel->setVisible(true);
  ui->pushButtonCancel->setEnabled(true);
  ui->pushButtonClose->setVisible(false);
  ui->pushButtonMinimize->setEnabled(true);
  this->show();
}

void ProgressDialogImp::setFinished()
{
  ui->pushButtonCancel->setVisible(false);
  ui->pushButtonClose->setVisible(true);
  ui->pushButtonClose->setText(tr("Close"));
  if (!this->isVisible() && !mAutoClose){
    this->show();
  } else if (mAutoClose) {
    this->close();
  }
  ui->pushButtonMinimize->setDisabled(true);
}

void ProgressDialogImp::setTitle(QString title)
{
  ui->progressBar->setWindowTitle(title);
}

void ProgressDialogImp::onMinimized()
{
  this->hide();
}

void ProgressDialogImp::onPushButtonCancelClicked()
{
  ui->pushButtonCancel->setEnabled(false);
  emit cancel();
  this->close();
}

} // namespace graphos