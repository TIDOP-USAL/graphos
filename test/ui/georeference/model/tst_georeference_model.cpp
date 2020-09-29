#include <QtTest>

#include "inspector/core/project.h"
#include "inspector/ui/georeference/impl/GeoreferenceModel.h"

using namespace inspector;
using namespace ui;

class TestGeoreferenceModel
  : public QObject
{
  Q_OBJECT

public:

  TestGeoreferenceModel();
  ~TestGeoreferenceModel();

private slots:

  void initTestCase();
  void cleanupTestCase();


protected:

  Project *mProject;
  GeoreferenceModel *mGeoreferenceModel;
};

TestGeoreferenceModel::TestGeoreferenceModel()
  : mProject(new ProjectImp)
{
  mProject->load(QString(INSPECTOR_SOURCE_PATH).append("/test/data/project.xml"));

  mGeoreferenceModel = new GeoreferenceModelImp(mProject);
}

TestGeoreferenceModel::~TestGeoreferenceModel()
{
  if (mProject){
    delete mProject;
    mProject = nullptr;
  }

 if (mGeoreferenceModel) {
   delete mGeoreferenceModel;
   mGeoreferenceModel = nullptr;
 }
}

void TestGeoreferenceModel::initTestCase()
{

}

void TestGeoreferenceModel::cleanupTestCase()
{
  mGeoreferenceModel->clear();

}


QTEST_APPLESS_MAIN(TestGeoreferenceModel)

#include "tst_georeference_model.moc"
