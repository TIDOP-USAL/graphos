#include "inspector/core/dtm/dtm.h"

namespace inspector
{

class DtmInvDistNNProperties
  : public DtmInvDistNN
{

public:

  DtmInvDistNNProperties();
  ~DtmInvDistNNProperties();

// DtmInvDistNN interface

public:

  double power() const override;
  double smoothing() const override;
  double radius() const override;
  int maxPoints() const override;
  int minPoints() const override;

  void setPower(double power) override;
  void setSmoothing(double smoothing) override;
  void setRadius(double radius) override;
  void setMaxPoints(int maxPoints) override;
  void setMinPoints(int minPoints) override;

// Dtm interface

public:

  std::string name() const override final;
  void reset() override;

protected:

  double mPower;
  double mSmoothing;
  double mRadius;
  int mMaxPoints;
  int mMinPoints;

};




class DtmInvDistNNAlgorithm
  : public DtmInvDistNNProperties,
    public DtmAlgorithm
{

public:

  DtmInvDistNNAlgorithm();
  DtmInvDistNNAlgorithm(double power,
                        double smoothing,
                        double radius,
                        int maxPoints,
                        int minPoints);
  ~DtmInvDistNNAlgorithm();

// DtmAlgorithm interface

public:

  bool run(const std::string &pointCloud,
           const std::string &dtmFile,
           const tl::Size<int> &size) override;

  bool run(const std::string &pointCloud,
           const std::string &dtmFile,
           const tl::BoundingBox<tl::Point3<double>> &bbox,
           double gsd) override;

};


} // namespace inspector
