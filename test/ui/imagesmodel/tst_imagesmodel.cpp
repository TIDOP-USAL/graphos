#include <QtTest>

#include "inspector/core/project.h"
#include "inspector/ui/ImagesModel.h"

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
  Image image("C:/Users/esteban/Documents/Inspector/Projects/SanSegundo/images/IMG_7213.jpg");
  image.setCameraId(1);
  image.setLongitudeExif(0.55);
  image.setLatitudeExif(2.3);
  image.setAltitudeExif(10.2);
  mImagesModel->addImage(image);

  Image image2 = mImagesModel->findImageByName("IMG_7213");
  QCOMPARE(QString("IMG_7213"), image2.name());
  QCOMPARE(1, image2.cameraId());
  QCOMPARE(0.55, image2.longitudeExif());
  QCOMPARE(2.3, image2.latitudeExif());
  QCOMPARE(10.2, image2.altitudeExif());
}

void TestImagesModel::test_updateImage()
{
  Image image1 = mImagesModel->findImageById(0);
  image1.setAltitudeExif(25.0);

  bool modified = mImagesModel->updateImage(0, image1);
  QCOMPARE(true, modified);

  Image image_modified = mImagesModel->findImageById(0);
  QCOMPARE(25.0, image_modified.altitudeExif());

  // Se recupera el estado original
  image1.setAltitudeExif(0);
  mImagesModel->updateImage(0, image1);
}

void TestImagesModel::test_deleteImage()
{
  Image image("C:/Users/esteban/Documents/Inspector/Projects/SanSegundo/images/IMG_7214.jpg");
  image.setCameraId(1);
  image.setLongitudeExif(0.66);
  image.setLatitudeExif(2.7);
  image.setAltitudeExif(10.1);

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
  QCOMPARE(image1.longitudeExif(), image2.longitudeExif());
  QCOMPARE(image1.latitudeExif(), image2.latitudeExif());
  QCOMPARE(image1.altitudeExif(), image2.altitudeExif());
  QCOMPARE(image1.cameraId(), image2.cameraId());
}

QTEST_APPLESS_MAIN(TestImagesModel)

#include "tst_imagesmodel.moc"
