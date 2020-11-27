#include "inspector/core/dtm/dtm.h"

namespace inspector
{

class DtmInvDistProperties
  : public DtmInvDist
{

public:

  DtmInvDistProperties();
  ~DtmInvDistProperties();

// DtmInvDist interface

public:

  double power() const override;
  double smoothing() const override;
  double radius1() const override;
  double radius2() const override;
  double angle() const override;
  int maxPoints() const override;
  int minPoints() const override;

  void setPower(double power) override;
  void setSmoothing(double smoothing) override;
  void setRadius1(double radius1) override;
  void setRadius2(double radius2) override;
  void setAngle(double angle) override;
  void setMaxPoints(int maxPoints) override;
  void setMinPoints(int minPoints) override;

// Dtm interface

public:

  QString name() const override final;
  void reset() override;

protected:

  double mPower;
  double mSmoothing;
  double mRadius1;
  double mRadius2;
  double mAngle;
  int mMaxPoints;
  int mMinPoints;

};




class DtmInvDistAlgorithm
  : public DtmInvDistProperties,
    public DtmAlgorithm
{

public:

  DtmInvDistAlgorithm();
  DtmInvDistAlgorithm(double power,
                    double smoothing,
                    double radius1,
                    double radius2,
                    double angle,
                    int maxPoints,
                    int minPoints);
  ~DtmInvDistAlgorithm();

// DtmAlgorithm interface

public:

  bool run(const QString &pointCloud,
           const QString &dtmFile,
           const tl::Size<int> &size) override;

};


} // namespace inspector
