#include <QDebug>
#include <QFileDialog>
#include <QFile>

#include "ProgressDialog.h"
#include "ui_ProgressDialog.h"


namespace inspector
{

namespace ui
{

ProgressDialogImp::ProgressDialogImp(QWidget *parent)
  : ProgressDialog(parent),
    ui(new Ui::ProgressDialog)
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
  ui->pushButtonClose->setVisible(false);
  ui->pushButtonMinimize->setEnabled(true);
  this->show();
}

void ProgressDialogImp::setFinished()
{
  ui->pushButtonCancel->setVisible(false);
  ui->pushButtonClose->setVisible(true);
  ui->pushButtonClose->setText(tr("Close"));
  if (!this->isVisible()){
    this->show();
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
  emit cancel();
  close();
}

} // namespace ui

} // namespace inspector