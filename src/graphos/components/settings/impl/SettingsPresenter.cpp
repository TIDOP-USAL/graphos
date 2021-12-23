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

#include "SettingsPresenter.h"

#include "graphos/components/settings/SettingsModel.h"
#include "graphos/components/settings/SettingsView.h"
#include "graphos/components/HelpDialog.h"

#include <QLocale>

namespace graphos
{


SettingsPresenterImp::SettingsPresenterImp(SettingsView *view,
                                           SettingsModel *model)
  : SettingsPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

//void SettingsPresenterImp::openViewSettings()
//{
//  mView->setPage(1);
//  this->open();
//}
//
//void SettingsPresenterImp::openToolSettings()
//{
//  mView->setPage(2);
//  this->open();
//}

void SettingsPresenterImp::help()
{
  if (mHelp){
    mHelp->setPage("settings.html");
    mHelp->setModal(true);
    mHelp->showMaximized();
  }
}

void SettingsPresenterImp::open()
{
  QStringList languages = mModel->languages();
  QStringList langs;
  mLang.clear();

  for (int i = 0; i < languages.size(); ++i) {
    QString lang_code = languages[i];
    lang_code.truncate(lang_code.lastIndexOf('.'));
    lang_code.remove(0, lang_code.indexOf('_') + 1);
    QString lang_name = QLocale::languageToString(QLocale(lang_code).language());
    langs.push_back(lang_name);
    mLang[lang_name] = lang_code;
  }

  mView->setLanguages(langs);

  mView->setHistoryMaxSize(mModel->historyMaxSize());
  mView->setUseCuda(mModel->useCuda() && mModel->checkDevice());
  mView->setCudaEnabled(mModel->checkDevice());
//#ifdef HAVE_CUDA
//  mView->setCudaEnabled(true);
//#else
//  mView->setCudaEnabled(false);
//#endif //HAVE_CUDA

  mView->exec();
}

void SettingsPresenterImp::setHelp(HelpDialog *help)
{
  mHelp = help;
}

void SettingsPresenterImp::init()
{

}

void SettingsPresenterImp::initSignalAndSlots()
{
  connect(mView, SIGNAL(languageChange(QString)),        this,   SLOT(setLanguage(QString)));
  connect(mView, SIGNAL(historyMaxSizeChange(int)),      mModel, SLOT(setHistoryMaxSize(int)));
  connect(mView, SIGNAL(useCudaChange(bool)),            mModel, SLOT(setUseCuda(bool)));
  connect(mView, SIGNAL(imageViewerBGColorChange(QString)),  mModel, SLOT(setImageViewerBGcolor(QString)));

  connect(mView, SIGNAL(accepted()), this, SLOT(save()));
  connect(mView, SIGNAL(applyChanges()), this, SLOT(save()));
  connect(mView, SIGNAL(rejected()), this, SLOT(discart()));
  connect(mView, SIGNAL(help()),     this, SLOT(help()));

  connect(mModel, SIGNAL(unsavedChanges(bool)), mView, SLOT(setUnsavedChanges(bool)));
}

void SettingsPresenterImp::setLanguage(const QString &language)
{
  QString lang_code = mLang[language];

  mModel->setLanguage(lang_code);
}

void SettingsPresenterImp::save()
{
  mModel->write();
}

void SettingsPresenterImp::discart()
{
  mModel->read();
}

} // namespace graphos
