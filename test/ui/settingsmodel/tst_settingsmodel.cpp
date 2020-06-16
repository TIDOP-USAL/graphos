#include <QtTest>

#include "inspector/ui/SettingsModel.h"
#include "fake/SettingsControllerFake.h"

using namespace inspector;
using namespace inspector::ui;


class TestSettingsModel
  : public QObject
{
  Q_OBJECT

public:

  TestSettingsModel();
  ~TestSettingsModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_defaultValues();
  void test_language();
  void test_languages();
  void test_history();

protected:

  Settings *mSettings;
  SettingsController *mSettingsController;
  SettingsModel *mSettingsModel;

};

TestSettingsModel::TestSettingsModel()
  : mSettings(new SettingsImp),
    mSettingsController(new SettingsControllerFake)
{
  mSettingsModel = new SettingsModelImp(mSettings, mSettingsController);
}

TestSettingsModel::~TestSettingsModel()
{
  if (mSettings){
    delete mSettings;
    mSettings = nullptr;
  }

  if (mSettingsController){
    delete mSettingsController;
    mSettingsController = nullptr;
  }

  if (mSettingsModel){
    delete mSettingsModel;
    mSettingsModel = nullptr;
  }
}

void TestSettingsModel::initTestCase()
{
  mSettingsModel->read();
}

void TestSettingsModel::cleanupTestCase()
{
  mSettingsModel->setLanguage("es");
  mSettingsModel->setHistoryMaxSize(20);
  mSettingsModel->setImageViewerBGcolor("#ff00ff");
  mSettingsModel->setUseCuda(true);

  mSettingsModel->clear();

  QCOMPARE("en", mSettingsModel->language());
  QCOMPARE(10, mSettingsModel->historyMaxSize());
  QCOMPARE(QStringList(), mSettingsModel->history());
  QCOMPARE(QString("#dcdcdc"), mSettingsModel->imageViewerBGcolor());

QSKIP("");
  QCOMPARE(true, mSettingsModel->useCuda());
}

void TestSettingsModel::test_defaultValues()
{
  SettingsImp settings;
  SettingsControllerFake rw;
  SettingsModelImp settingsModel(&settings, &rw);

  QCOMPARE("en", settingsModel.language());

  QCOMPARE(10, settingsModel.historyMaxSize());
  QCOMPARE(QStringList(), settingsModel.history());

  QCOMPARE(QString("#dcdcdc"), settingsModel.imageViewerBGcolor());

  QCOMPARE(false, settingsModel.useCuda());
}

void TestSettingsModel::test_language()
{
  QCOMPARE("en", mSettingsModel->language());

  mSettingsModel->setLanguage("es");
  QCOMPARE("es", mSettingsModel->language());
}

void TestSettingsModel::test_languages()
{
  /// Sólo se chequea el valor por defecto (si no existen ficheros de idioma)
  QStringList languages = mSettingsModel->languages();
  QCOMPARE("photomatch_en.qm", languages[0]);
}

void TestSettingsModel::test_history()
{
  mSettingsModel->setHistoryMaxSize(8);
  QCOMPARE(8, mSettingsModel->historyMaxSize());

  /// Se añaden proyectos al historial

  mSettingsModel->addToHistory("c://projects/prj01.xml");
  mSettingsModel->addToHistory("c://projects/prj02.xml");
  mSettingsModel->addToHistory("c://projects/prj03.xml");
  mSettingsModel->addToHistory("c://projects/prj04.xml");
  mSettingsModel->addToHistory("c://projects/prj05.xml");
  mSettingsModel->addToHistory("c://projects/prj06.xml");
  mSettingsModel->addToHistory("c://projects/prj07.xml");
  mSettingsModel->addToHistory("c://projects/prj08.xml");
  mSettingsModel->addToHistory("c://projects/prj09.xml");
  mSettingsModel->addToHistory("c://projects/prj10.xml");
  mSettingsModel->addToHistory("c://projects/prj11.xml");

  /// Se recupera el historial
  QStringList h = mSettingsModel->history();
  QCOMPARE(8, h.size());
  QCOMPARE("c://projects/prj11.xml", h[0]);

  /// Se borra el historial
  mSettingsModel->clearHistory();
  h = mSettings->history();
  QCOMPARE(0, h.size());
}


QTEST_MAIN(TestSettingsModel)

#include "tst_settingsmodel.moc"
