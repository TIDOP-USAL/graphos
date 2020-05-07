#include "SettingsPresenter.h"

#include "SettingsModel.h"
#include "SettingsView.h"

#include <QLocale>

namespace inspector
{

namespace ui
{


SettingsPresenterImp::SettingsPresenterImp(SettingsView *view, SettingsModel *model)
  : SettingsPresenter(),
    mView(view),
    mModel(model),
    mHelp(nullptr)
{
  this->init();
  this->initSignalAndSlots();
}

SettingsPresenterImp::~SettingsPresenterImp()
{

}

void SettingsPresenterImp::openViewSettings()
{
  mView->setPage(1);
  this->open();
}

void SettingsPresenterImp::openToolSettings()
{
  mView->setPage(2);
  this->open();
}

void SettingsPresenterImp::help()
{
//  if (mHelp){
//    mHelp->setPage("settings.html");
//    mHelp->show();
//  }
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
  mView->setUseCuda(mModel->useCuda());
  mView->setCudaEnabled(false);
#ifdef HAVE_CUDA
  mView->setCudaEnabled(true);
#else
  mView->setCudaEnabled(false);
#endif //HAVE_CUDA

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

} // namespace ui

} // namespace inspector
