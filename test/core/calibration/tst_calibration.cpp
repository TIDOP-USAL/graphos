#include <QtTest>

#include "inspector/core/camera.h"


using namespace inspector;

class TestCalibration
  : public QObject
{
  Q_OBJECT

public:

  TestCalibration();
  ~TestCalibration();

private slots:

  void initTestCase();
  void cleanupTestCase();
  void test_name();
  void test_parametersBegin();
  void test_parameter();

protected:

  std::shared_ptr<Calibration> mCalibrationRadial1;
  std::shared_ptr<Calibration> mCalibrationRadial2;
  std::shared_ptr<Calibration> mCalibrationRadial3;
  std::shared_ptr<Calibration> mCalibrationSimpleRadialFisheye;
  std::shared_ptr<Calibration> mCalibrationRadialFisheye;
  std::shared_ptr<Calibration> mCalibrationOpenCV;
  std::shared_ptr<Calibration> mCalibrationOpenCVFisheye;
  std::shared_ptr<Calibration> mCalibrationOpenCVFull;
  std::shared_ptr<Calibration> mCalibrationSimplePinhole;
  std::shared_ptr<Calibration> mCalibrationPinhole;
};

TestCalibration::TestCalibration()
{
  mCalibrationRadial1 = Calibration::create(Calibration::CameraModel::radial1);
  mCalibrationRadial1->setParameter(Calibration::Parameters::focal, 4000);
  mCalibrationRadial1->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationRadial1->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationRadial1->setParameter(Calibration::Parameters::k1, 0.005);

  mCalibrationRadial2 = Calibration::create(Calibration::CameraModel::radial2);
  mCalibrationRadial2->setParameter(Calibration::Parameters::focal, 4000);
  mCalibrationRadial2->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationRadial2->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationRadial2->setParameter(Calibration::Parameters::k1, 0.005);
  mCalibrationRadial2->setParameter(Calibration::Parameters::k2, 0.001);

  mCalibrationRadial3 = Calibration::create(Calibration::CameraModel::radial3);
  mCalibrationRadial3->setParameter(Calibration::Parameters::focal, 4000);
  mCalibrationRadial3->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationRadial3->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationRadial3->setParameter(Calibration::Parameters::k1, 0.005);
  mCalibrationRadial3->setParameter(Calibration::Parameters::k2, 0.001);
  mCalibrationRadial3->setParameter(Calibration::Parameters::k3, 0.0001);
  mCalibrationRadial3->setParameter(Calibration::Parameters::p1, 0.01);
  mCalibrationRadial3->setParameter(Calibration::Parameters::p2, 0.001);

  mCalibrationSimpleRadialFisheye = Calibration::create(Calibration::CameraModel::simple_radial_fisheye);
  mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::focal, 4000);
  mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationSimpleRadialFisheye->setParameter(Calibration::Parameters::k1, 0.005);

  mCalibrationRadialFisheye = Calibration::create(Calibration::CameraModel::radial_fisheye);
  mCalibrationRadialFisheye->setParameter(Calibration::Parameters::focal, 4000);
  mCalibrationRadialFisheye->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationRadialFisheye->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationRadialFisheye->setParameter(Calibration::Parameters::k1, 0.005);
  mCalibrationRadialFisheye->setParameter(Calibration::Parameters::k2, 0.001);

  mCalibrationOpenCV = Calibration::create(Calibration::CameraModel::opencv);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::focalx, 4000);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::focaly, 4000);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::k1, 0.005);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::k2, 0.001);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::p1, 0.01);
  mCalibrationOpenCV->setParameter(Calibration::Parameters::p2, 0.001);

  mCalibrationOpenCVFisheye = Calibration::create(Calibration::CameraModel::opencv_fisheye);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::focalx, 4000);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::focaly, 4000);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k1, 0.005);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k2, 0.001);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k3, 0.0005);
  mCalibrationOpenCVFisheye->setParameter(Calibration::Parameters::k4, 0.0001);

  mCalibrationOpenCVFull = Calibration::create(Calibration::CameraModel::opencv_full);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::focalx, 4000);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::focaly, 4000);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::cy, 0.5);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k1, 0.005);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k2, 0.001);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k3, 0.0005);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k4, 0.0001);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k5, 0.00005);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::k6, 0.00001);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::p1, 0.01);
  mCalibrationOpenCVFull->setParameter(Calibration::Parameters::p2, 0.001);

  mCalibrationSimplePinhole = Calibration::create(Calibration::CameraModel::simple_pinhole);
  mCalibrationSimplePinhole->setParameter(Calibration::Parameters::focal, 4000);
  mCalibrationSimplePinhole->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationSimplePinhole->setParameter(Calibration::Parameters::cy, 0.5);

  mCalibrationPinhole = Calibration::create(Calibration::CameraModel::pinhole);
  mCalibrationPinhole->setParameter(Calibration::Parameters::focalx, 4000);
  mCalibrationPinhole->setParameter(Calibration::Parameters::focaly, 4000);
  mCalibrationPinhole->setParameter(Calibration::Parameters::cx, 0.6);
  mCalibrationPinhole->setParameter(Calibration::Parameters::cy, 0.5);
}

TestCalibration::~TestCalibration()
{
}

void TestCalibration::initTestCase()
{

}

void TestCalibration::cleanupTestCase()
{

}

void TestCalibration::test_name()
{
  QCOMPARE("Simple radial", mCalibrationRadial1->name());
  QCOMPARE("Radial", mCalibrationRadial2->name());
  QCOMPARE("Full radial", mCalibrationRadial3->name());
  QCOMPARE("Simple Radial Fisheye", mCalibrationSimpleRadialFisheye->name());
  QCOMPARE("Radial Fisheye", mCalibrationRadialFisheye->name());
  QCOMPARE("OpenCV", mCalibrationOpenCV->name());
  QCOMPARE("OpenCV Fisheye", mCalibrationOpenCVFisheye->name());
  QCOMPARE("OpenCV Full", mCalibrationOpenCVFull->name());
  QCOMPARE("Simple Pinhole", mCalibrationSimplePinhole->name());
  QCOMPARE("Pinhole", mCalibrationPinhole->name());
}

void TestCalibration::test_parametersBegin()
{
  Calibration::parameter_iterator it = mCalibrationRadial1->parametersBegin();

  QCOMPARE(Calibration::Parameters::focal, it->first);
  QCOMPARE(4000., it->second);

  it++;
  QCOMPARE(Calibration::Parameters::cx, it->first);
  QCOMPARE(0.6, it->second);

  it++;
  QCOMPARE(Calibration::Parameters::cy, it->first);
  QCOMPARE(0.5, it->second);

  it++;
  QCOMPARE(Calibration::Parameters::k1, it->first);
  QCOMPARE(0.005, it->second);

  Calibration::parameter_iterator it2 = mCalibrationRadial2->parametersBegin();

  QCOMPARE(Calibration::Parameters::focal, it2->first);
  QCOMPARE(4000., it2->second);

  it2++;
  QCOMPARE(Calibration::Parameters::cx, it2->first);
  QCOMPARE(0.6, it2->second);

  it2++;
  QCOMPARE(Calibration::Parameters::cy, it2->first);
  QCOMPARE(0.5, it2->second);

  it2++;
  QCOMPARE(Calibration::Parameters::k1, it2->first);
  QCOMPARE(0.005, it2->second);

  it2++;
  QCOMPARE(Calibration::Parameters::k2, it2->first);
  QCOMPARE(0.001, it2->second);

  Calibration::parameter_iterator it3 = mCalibrationRadial3->parametersBegin();

  QCOMPARE(Calibration::Parameters::focal, it3->first);
  QCOMPARE(4000., it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::cx, it3->first);
  QCOMPARE(0.6, it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::cy, it3->first);
  QCOMPARE(0.5, it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::k1, it3->first);
  QCOMPARE(0.005, it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::k2, it3->first);
  QCOMPARE(0.001, it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::k3, it3->first);
  QCOMPARE(0.0001, it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::p1, it3->first);
  QCOMPARE(0.01, it3->second);

  it3++;
  QCOMPARE(Calibration::Parameters::p2, it3->first);
  QCOMPARE(0.001, it3->second);

  Calibration::parameter_iterator it4 = mCalibrationSimpleRadialFisheye->parametersBegin();

  QCOMPARE(Calibration::Parameters::focal, it4->first);
  QCOMPARE(4000., it4->second);

  it4++;
  QCOMPARE(Calibration::Parameters::cx, it4->first);
  QCOMPARE(0.6, it4->second);

  it4++;
  QCOMPARE(Calibration::Parameters::cy, it4->first);
  QCOMPARE(0.5, it4->second);

  it4++;
  QCOMPARE(Calibration::Parameters::k1, it4->first);
  QCOMPARE(0.005, it4->second);

  Calibration::parameter_iterator it5 = mCalibrationRadialFisheye->parametersBegin();

  QCOMPARE(Calibration::Parameters::focal, it5->first);
  QCOMPARE(4000., it5->second);

  it5++;
  QCOMPARE(Calibration::Parameters::cx, it5->first);
  QCOMPARE(0.6, it5->second);

  it5++;
  QCOMPARE(Calibration::Parameters::cy, it5->first);
  QCOMPARE(0.5, it5->second);

  it5++;
  QCOMPARE(Calibration::Parameters::k1, it5->first);
  QCOMPARE(0.005, it5->second);

  it5++;
  QCOMPARE(Calibration::Parameters::k2, it5->first);
  QCOMPARE(0.001, it5->second);

  Calibration::parameter_iterator it6 = mCalibrationOpenCV->parametersBegin();

  QCOMPARE(Calibration::Parameters::focalx, it6->first);
  QCOMPARE(4000., it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::focaly, it6->first);
  QCOMPARE(4000., it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::cx, it6->first);
  QCOMPARE(0.6, it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::cy, it6->first);
  QCOMPARE(0.5, it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::k1, it6->first);
  QCOMPARE(0.005, it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::k2, it6->first);
  QCOMPARE(0.001, it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::p1, it6->first);
  QCOMPARE(0.01, it6->second);

  it6++;
  QCOMPARE(Calibration::Parameters::p2, it6->first);
  QCOMPARE(0.001, it6->second);

  Calibration::parameter_iterator it7 = mCalibrationOpenCVFisheye->parametersBegin();

  QCOMPARE(Calibration::Parameters::focalx, it7->first);
  QCOMPARE(4000., it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::focaly, it7->first);
  QCOMPARE(4000., it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::cx, it7->first);
  QCOMPARE(0.6, it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::cy, it7->first);
  QCOMPARE(0.5, it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::k1, it7->first);
  QCOMPARE(0.005, it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::k2, it7->first);
  QCOMPARE(0.001, it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::k3, it7->first);
  QCOMPARE(0.0005, it7->second);

  it7++;
  QCOMPARE(Calibration::Parameters::k4, it7->first);
  QCOMPARE(0.0001, it7->second);

  Calibration::parameter_iterator it8 = mCalibrationOpenCVFull->parametersBegin();

  QCOMPARE(Calibration::Parameters::focalx, it8->first);
  QCOMPARE(4000., it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::focaly, it8->first);
  QCOMPARE(4000., it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::cx, it8->first);
  QCOMPARE(0.6, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::cy, it8->first);
  QCOMPARE(0.5, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::k1, it8->first);
  QCOMPARE(0.005, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::k2, it8->first);
  QCOMPARE(0.001, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::k3, it8->first);
  QCOMPARE(0.0005, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::k4, it8->first);
  QCOMPARE(0.0001, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::k5, it8->first);
  QCOMPARE(0.00005, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::k6, it8->first);
  QCOMPARE(0.00001, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::p1, it8->first);
  QCOMPARE(0.01, it8->second);

  it8++;
  QCOMPARE(Calibration::Parameters::p2, it8->first);
  QCOMPARE(0.001, it8->second);

  Calibration::parameter_iterator it9 = mCalibrationSimplePinhole->parametersBegin();

  QCOMPARE(Calibration::Parameters::focal, it9->first);
  QCOMPARE(4000., it9->second);

  it9++;
  QCOMPARE(Calibration::Parameters::cx, it9->first);
  QCOMPARE(0.6, it9->second);

  it9++;
  QCOMPARE(Calibration::Parameters::cy, it9->first);
  QCOMPARE(0.5, it9->second);

  Calibration::parameter_iterator it10 = mCalibrationPinhole->parametersBegin();

  QCOMPARE(Calibration::Parameters::focalx, it10->first);
  QCOMPARE(4000., it10->second);

  it10++;
  QCOMPARE(Calibration::Parameters::focaly, it10->first);
  QCOMPARE(4000., it10->second);

  it10++;
  QCOMPARE(Calibration::Parameters::cx, it10->first);
  QCOMPARE(0.6, it10->second);

  it10++;
  QCOMPARE(Calibration::Parameters::cy, it10->first);
  QCOMPARE(0.5, it10->second);
}

void TestCalibration::test_parameter()
{
  QCOMPARE(4000, mCalibrationRadial1->parameter(Calibration::Parameters::focal));
  QCOMPARE(0.6, mCalibrationRadial1->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationRadial1->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationRadial1->parameter(Calibration::Parameters::k1));

  QCOMPARE(4000, mCalibrationRadial2->parameter(Calibration::Parameters::focal));
  QCOMPARE(0.6, mCalibrationRadial2->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationRadial2->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationRadial2->parameter(Calibration::Parameters::k1));
  QCOMPARE(0.001, mCalibrationRadial2->parameter(Calibration::Parameters::k2));
  QCOMPARE(4000, mCalibrationRadial3->parameter(Calibration::Parameters::focal));
  QCOMPARE(0.6, mCalibrationRadial3->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationRadial3->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationRadial3->parameter(Calibration::Parameters::k1));
  QCOMPARE(0.001, mCalibrationRadial3->parameter(Calibration::Parameters::k2));
  QCOMPARE(0.0001, mCalibrationRadial3->parameter(Calibration::Parameters::k3));
  QCOMPARE(0.01, mCalibrationRadial3->parameter(Calibration::Parameters::p1));
  QCOMPARE(0.001, mCalibrationRadial3->parameter(Calibration::Parameters::p2));
  QCOMPARE(4000, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::focal));
  QCOMPARE(0.6, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationSimpleRadialFisheye->parameter(Calibration::Parameters::k1));
  QCOMPARE(4000, mCalibrationRadialFisheye->parameter(Calibration::Parameters::focal));
  QCOMPARE(0.6, mCalibrationRadialFisheye->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationRadialFisheye->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationRadialFisheye->parameter(Calibration::Parameters::k1));
  QCOMPARE(0.001, mCalibrationRadialFisheye->parameter(Calibration::Parameters::k2));
  QCOMPARE(4000, mCalibrationOpenCV->parameter(Calibration::Parameters::focalx));
  QCOMPARE(4000, mCalibrationOpenCV->parameter(Calibration::Parameters::focaly));
  QCOMPARE(0.6, mCalibrationOpenCV->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationOpenCV->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationOpenCV->parameter(Calibration::Parameters::k1));
  QCOMPARE(0.001, mCalibrationOpenCV->parameter(Calibration::Parameters::k2));
  QCOMPARE(0.01, mCalibrationOpenCV->parameter(Calibration::Parameters::p1));
  QCOMPARE(0.001, mCalibrationOpenCV->parameter(Calibration::Parameters::p2));
  QCOMPARE(4000, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::focalx));
  QCOMPARE(4000, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::focaly));
  QCOMPARE(0.6, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k1));
  QCOMPARE(0.001, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k2));
  QCOMPARE(0.0005, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k3));
  QCOMPARE(0.0001, mCalibrationOpenCVFisheye->parameter(Calibration::Parameters::k4));
  QCOMPARE(4000, mCalibrationOpenCVFull->parameter(Calibration::Parameters::focalx));
  QCOMPARE(4000, mCalibrationOpenCVFull->parameter(Calibration::Parameters::focaly));
  QCOMPARE(0.6, mCalibrationOpenCVFull->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationOpenCVFull->parameter(Calibration::Parameters::cy));
  QCOMPARE(0.005, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k1));
  QCOMPARE(0.001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k2));
  QCOMPARE(0.0005, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k3));
  QCOMPARE(0.0001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k4));
  QCOMPARE(0.00005, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k5));
  QCOMPARE(0.00001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::k6));
  QCOMPARE(0.01, mCalibrationOpenCVFull->parameter(Calibration::Parameters::p1));
  QCOMPARE(0.001, mCalibrationOpenCVFull->parameter(Calibration::Parameters::p2));
  QCOMPARE(4000, mCalibrationSimplePinhole->parameter(Calibration::Parameters::focal));
  QCOMPARE(0.6, mCalibrationSimplePinhole->parameter(Calibration::Parameters::cx));
  QCOMPARE(0.5, mCalibrationSimplePinhole->parameter(Calibration::Parameters::cy));
  QCOMPARE(4000, mCalibrationPinhole->parameter(Calibration::Parameters::focalx));
  QCOMPARE(4000, mCalibrationPinhole->parameter(Calibration::Parameters::focaly));
  QCOMPARE(0.6, mCalibrationPinhole->parameter(Calibration::Parameters::cx));
  QCOMPARE( 0.5, mCalibrationPinhole->parameter(Calibration::Parameters::cy));

}

QTEST_APPLESS_MAIN(TestCalibration)

#include "tst_calibration.moc"
