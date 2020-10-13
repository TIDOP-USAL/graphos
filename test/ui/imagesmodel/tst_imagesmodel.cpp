#include <QtTest>

#include "inspector/core/project.h"
#include "inspector/ui/images/ImagesModel.h"

using namespace inspector;
using namespace ui;

class TestImagesModel 
  : public QObject
{
  Q_OBJECT

public:

  TestImagesModel();
  ~TestImagesModel();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_addImage();
  void test_updateImage();
  void test_deleteImage();
  void test_imageId();
  void test_findImage();

protected:

  Project *mProject;
  ImagesModel *mImagesModel;
};

TestImagesModel::TestImagesModel()
  : mProject(new ProjectImp)
{
  mProject->load(QString(INSPECTOR_SOURCE_PATH).append("/test/data/project.xml"));

  mImagesModel = new ImagesModelImp(mProject);
}

TestImagesModel::~TestImagesModel()
{
  if (mProject){
    delete mProject;
    mProject = nullptr;
  }

 if (mImagesModel) {
   delete mImagesModel;
   mImagesModel = nullptr;
 }
}

void TestImagesModel::initTestCase()
{

}

void TestImagesModel::cleanupTestCase()
{
  mImagesModel->clear();

  QCOMPARE(mImagesModel->begin(), mImagesModel->end());

}

void TestImagesModel::test_addImage()
{
  Image image("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7213.jpg");
  image.setCameraId(1);
  CameraPosition cameraPosition2;
  cameraPosition2.setX(0.5);
  cameraPosition2.setY(2.3);
  cameraPosition2.setZ(10.2);
  image.setCameraPosition(cameraPosition2);
  mImagesModel->addImage(image);

  Image image2 = mImagesModel->findImageByName("IMG_7213");
  QCOMPARE(QString("IMG_7213"), image2.name());
  QCOMPARE(1, image2.cameraId());
  CameraPosition cameraPosition = image2.cameraPosition();
  QCOMPARE(0.5, cameraPosition.x());
  QCOMPARE(2.3, cameraPosition.y());
  QCOMPARE(10.2, cameraPosition.z());
}

void TestImagesModel::test_updateImage()
{
  Image image1 = mImagesModel->findImageById(0);
  CameraPosition cameraPosition = image1.cameraPosition();
  double z = cameraPosition.z();
  cameraPosition.setZ(25.0);
  image1.setCameraPosition(cameraPosition);

  bool modified = mImagesModel->updateImage(0, image1);
  QCOMPARE(true, modified);

  Image image_modified = mImagesModel->findImageById(0);
  cameraPosition = image_modified.cameraPosition();
  QCOMPARE(25.0, cameraPosition.z());

  // Se recupera el estado original
  cameraPosition.setZ(z);
  image1.setCameraPosition(cameraPosition);
  mImagesModel->updateImage(0, image1);
}

void TestImagesModel::test_deleteImage()
{
  Image image("C:/Users/User01/Documents/inspector/Projects/prj002/images/IMG_7214.jpg");
  image.setCameraId(1);

  mImagesModel->addImage(image);

  size_t id = mImagesModel->imageID("IMG_7214");

  bool removed = mImagesModel->removeImage(id);
  QCOMPARE(true, removed);

  removed = mImagesModel->removeImage(id);
  QCOMPARE(false, removed);
}

void TestImagesModel::test_imageId()
{
  size_t id = mImagesModel->imageID("IMG_7209");
  QCOMPARE(0, id);

  id = mImagesModel->imageID("IMG_7210");
  QCOMPARE(1, id);
}

void TestImagesModel::test_findImage()
{
  Image image1 = mImagesModel->findImageById(0);
  Image image2 = mImagesModel->findImageByName("IMG_7209");

  QCOMPARE(image1.name(), image2.name());
  QCOMPARE(image1.cameraId(), image2.cameraId());
  QCOMPARE(image1.cameraPosition().x(), image2.cameraPosition().x());
  QCOMPARE(image1.cameraPosition().y(), image2.cameraPosition().y());
  QCOMPARE(image1.cameraPosition().z(), image2.cameraPosition().z());
  QCOMPARE(image1.cameraId(), image2.cameraId());
}

QTEST_APPLESS_MAIN(TestImagesModel)

#include "tst_imagesmodel.moc"
