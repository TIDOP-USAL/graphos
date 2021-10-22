#include <QtTest>

#include "graphos/ui/openproject/impl/OpenProjectModel.h"
#include "graphos/core/project.h"
#include "../test/fake/ProjectFake.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QTextEdit>

using namespace graphos;


class TestOpenProjectModel
  : public QObject
{
  Q_OBJECT

public:

  TestOpenProjectModel();
  ~TestOpenProjectModel();

private slots:

  void initTestCase();
  void cleanupTestCase();

private:
  Project *project;
  OpenProjectModel *mProjectModel;
};

TestOpenProjectModel::TestOpenProjectModel()
  : project(new ProjectFake),
    mProjectModel(new OpenProjectModelImp(project))
{
}

TestOpenProjectModel::~TestOpenProjectModel()
{
}

void TestOpenProjectModel::initTestCase()
{
  QString inspector_directory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  inspector_directory.append("/graphos/Projects");

  QCOMPARE(inspector_directory, mProjectModel->inspectorDirectory());
}

void TestOpenProjectModel::cleanupTestCase()
{

}

QTEST_APPLESS_MAIN(TestOpenProjectModel)

#include "tst_openprojectmodel.moc"
