#include <QtTest>

#include "inspector/core/settings.h"

using namespace inspector;

class TestSettings 
  : public QObject
{
  Q_OBJECT

public:

  TestSettings();
  ~TestSettings();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_language_data();
  void test_language();
  void test_historyMaxSize_data();
  void test_historyMaxSize();
  void test_imageViewerBGcolor_data();
  void test_imageViewerBGcolor();
  void test_useCuda();

  void test_history();

protected:

  Settings *mSettings;
};

TestSettings::TestSettings()
  : mSettings(new SettingsImp)
{

}

TestSettings::~TestSettings()
{

}

void TestSettings::initTestCase()
{
  QCOMPARE("en", mSettings->language());
  QCOMPARE(10, mSettings->historyMaxSize());
  QCOMPARE(QStringList(), mSettings->history());
  QCOMPARE(QString("#dcdcdc"), mSettings->imageViewerBGcolor());
  QCOMPARE(false, mSettings->useCuda());
}

void TestSettings::cleanupTestCase()
{
  mSettings->setLanguage("es");
  mSettings->setHistoryMaxSize(20);
  mSettings->setImageViewerBGcolor("#ff00ff");
  mSettings->setUseCuda(true);

  mSettings->reset();

  QCOMPARE("en", mSettings->language());
  QCOMPARE(10, mSettings->historyMaxSize());
  QCOMPARE(QStringList(), mSettings->history());
  QCOMPARE(QString("#dcdcdc"), mSettings->imageViewerBGcolor());
  QCOMPARE(false, mSettings->useCuda());
}

void TestSettings::test_language_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("en") << "en" << "en";
  QTest::newRow("es") << "es" << "es";
}

void TestSettings::test_language()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mSettings->setLanguage(value);
  QCOMPARE(result, mSettings->language());
}

void TestSettings::test_historyMaxSize_data()
{
  QTest::addColumn<int>("value");
  QTest::addColumn<int>("result");

  QTest::newRow("1") << 1 << 1;
  QTest::newRow("5") << 5 << 5;
  QTest::newRow("10") << 10 << 10;
}

void TestSettings::test_historyMaxSize()
{
  QFETCH(int, value);
  QFETCH(int, result);

  mSettings->setHistoryMaxSize(value);
  QCOMPARE(result, mSettings->historyMaxSize());
}

void TestSettings::test_imageViewerBGcolor_data()
{
  QTest::addColumn<QString>("value");
  QTest::addColumn<QString>("result");

  QTest::newRow("#FF00FF") << "#FF00FF" << "#FF00FF";
  QTest::newRow("#253612") << "#253612" << "#253612";
  QTest::newRow("#205060") << "#205060" << "#205060";
}

void TestSettings::test_imageViewerBGcolor()
{
  QFETCH(QString, value);
  QFETCH(QString, result);

  mSettings->setImageViewerBGcolor(value);
  QCOMPARE(result, mSettings->imageViewerBGcolor());
}

void TestSettings::test_useCuda()
{
  mSettings->setUseCuda(true);
  QCOMPARE(true, mSettings->useCuda());

  mSettings->setUseCuda(false);
  QCOMPARE(false, mSettings->useCuda());
}

void TestSettings::test_history()
{
  mSettings->setHistoryMaxSize(8);
  QCOMPARE(8, mSettings->historyMaxSize());

  /// Se añaden proyectos al historial

  mSettings->addToHistory("c://projects/prj01.xml");
  mSettings->addToHistory("c://projects/prj02.xml");
  mSettings->addToHistory("c://projects/prj03.xml");
  mSettings->addToHistory("c://projects/prj04.xml");
  mSettings->addToHistory("c://projects/prj05.xml");
  mSettings->addToHistory("c://projects/prj06.xml");
  mSettings->addToHistory("c://projects/prj07.xml");
  mSettings->addToHistory("c://projects/prj08.xml");
  mSettings->addToHistory("c://projects/prj09.xml");
  mSettings->addToHistory("c://projects/prj10.xml");
  mSettings->addToHistory("c://projects/prj11.xml");

  /// Se recupera el historial
  QStringList h = mSettings->history();
  QCOMPARE(8, h.size());
  QCOMPARE("c://projects/prj11.xml", h[0]);

  /// Se borra el historial
  mSettings->clearHistory();
  h = mSettings->history();
  QCOMPARE(0, h.size());
}


QTEST_APPLESS_MAIN(TestSettings)

#include "tst_settings.moc"
