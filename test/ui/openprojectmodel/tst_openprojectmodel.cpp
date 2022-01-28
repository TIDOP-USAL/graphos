#include <QtTest>

#include "graphos/components/openproject/impl/OpenProjectModel.h"
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
  QString graphos_directory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  graphos_directory.append("/graphos/Projects");

  QCOMPARE(graphos_directory, mProjectModel->graphosProjectsDirectory());
}

void TestOpenProjectModel::cleanupTestCase()
{

}

QTEST_APPLESS_MAIN(TestOpenProjectModel)

#include "tst_openprojectmodel.moc"
