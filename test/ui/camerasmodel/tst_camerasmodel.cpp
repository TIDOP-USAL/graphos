#include <QtTest>

#include "graphos/core/project.h"
#include "graphos/components/cameras/CamerasModel.h"
//#include "fake/ProjectFake.h"

using namespace graphos;

class TestCamerasModel : public QObject
{
  Q_OBJECT

public:

  TestCamerasModel();
  ~TestCamerasModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_updateCamera();
  void test_deleteCamera();
  void test_cameraId();
  void test_findCamera();
  void test_findCamera_exception();
  void test_iterator();

protected:

  Project *mProject;
  CamerasModel *mCamerasModel;
};

TestCamerasModel::TestCamerasModel()
  : mProject(new ProjectImp)
{

  mProject->load(QString(INSPECTOR_SOURCE_PATH).append("/test/data/project.xml"));

  mCamerasModel = new CamerasModelImp(mProject);
}

TestCamerasModel::~TestCamerasModel()
{
  if (mProject){
    delete mProject;
    mProject = nullptr;
  }

  if (mCamerasModel) {
    delete mCamerasModel;
    mCamerasModel = nullptr;
  }
}

void TestCamerasModel::initTestCase()
{
  Camera camera("DJI", "FC6310");
  camera.setType("Simple radial");
  camera.setFocal(3552.23);
  camera.setWidth(5472);
  camera.setHeight(3648);
  camera.setSensorSize(12.8333);
  int id = mCamerasModel->addCamera(camera);
  QCOMPARE(2, id);
}

void TestCamerasModel::cleanupTestCase()
{
  mCamerasModel->clear();

  QCOMPARE(mCamerasModel->begin(), mCamerasModel->end());
}

void TestCamerasModel::test_updateCamera()
{
  Camera camera = mCamerasModel->camera(1);
  camera.setType("Full radial");
  bool bUpdate = mCamerasModel->updateCamera(1, camera);

  QCOMPARE(true, bUpdate);

  camera = mCamerasModel->camera(1);

  QCOMPARE(QString("Full radial"), camera.type());

  /// Se recupera el estado anterior
  camera.setType("Radial");
  mCamerasModel->updateCamera(1, camera);
}

void TestCamerasModel::test_deleteCamera()
{
  Camera camera("SONY", "ILCE-6000");
  int id = mCamerasModel->addCamera(camera);

  bool remove = mCamerasModel->removeCamera(4);
  QCOMPARE(false, remove);

  remove = mCamerasModel->removeCamera(id);
  QCOMPARE(true, remove);
}

void TestCamerasModel::test_cameraId()
{
  int id = mCamerasModel->cameraID("Unknown camera", "0");
  QCOMPARE(1, id);

  id = mCamerasModel->cameraID("DJI", "FC6310");
  QCOMPARE(2, id);

  id = mCamerasModel->cameraID("Sony", "ILCE-6000");
  QCOMPARE(0, id);
}

void TestCamerasModel::test_findCamera()
{
  Camera camera = mCamerasModel->camera(1);

  QCOMPARE(QString("Unknown camera"), camera.make());
  QCOMPARE(QString("0"), camera.model());
  QCOMPARE(QString("Radial"), camera.type());
  QCOMPARE(5835.6, camera.focal());
  QCOMPARE(4863, camera.width());
  QCOMPARE(3221, camera.height());
  QCOMPARE(1., camera.sensorSize());

  camera = mCamerasModel->camera("Unknown camera", "0");

  QCOMPARE(QString("0"), camera.model());
  QCOMPARE(QString("Radial"), camera.type());
  QCOMPARE(5835.6, camera.focal());
  QCOMPARE(4863, camera.width());
  QCOMPARE(3221, camera.height());
  QCOMPARE(1., camera.sensorSize());
}

void TestCamerasModel::test_findCamera_exception()
{
  try {
    Camera camera = mCamerasModel->camera(5);
  } catch (std::exception &e) {
    QCOMPARE("Camera not exist", e.what());
  }
}

void TestCamerasModel::test_iterator()
{
  auto it = mCamerasModel->begin();
  int id = it->first;
  Camera camera = it->second;

  QCOMPARE(1, id);
  QCOMPARE(QString("Unknown camera"), camera.make());
  QCOMPARE(QString("0"), camera.model());
  QCOMPARE(QString("Radial"), camera.type());
  QCOMPARE(5835.6, camera.focal());
  QCOMPARE(4863, camera.width());
  QCOMPARE(3221, camera.height());
  QCOMPARE(1., camera.sensorSize());

  it++;

  id = it->first;
  camera = it->second;

  QCOMPARE(2, id);
  QCOMPARE(QString("DJI"), camera.make());
  QCOMPARE(QString("FC6310"), camera.model());
  QCOMPARE(QString("Simple radial"), camera.type());
  QCOMPARE(3552.23, camera.focal());
  QCOMPARE(5472, camera.width());
  QCOMPARE(3648, camera.height());
  QCOMPARE(12.8333, camera.sensorSize());

}


QTEST_APPLESS_MAIN(TestCamerasModel)

#include "tst_camerasmodel.moc"
