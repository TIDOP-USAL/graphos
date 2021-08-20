#ifndef FEATUREMATCHINGWIDGET_H
#define FEATUREMATCHINGWIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{

class GRAPHOS_EXPORT FeatureMatchingWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  FeatureMatchingWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~FeatureMatchingWidget() override = default;

  virtual bool crossCheck() const = 0;
  virtual double ratio() const = 0;
  virtual double distance() const = 0;
  //virtual int maxMatches() const = 0;
  virtual double maxError() const = 0;
  virtual double confidence() const = 0;

signals:

  void crossCheckChanged(bool);
  void ratioChanged(double);
  void distanceChanged(double);
  //void maxMachesChanged(int);
  void maxErrorChanged(double);
  void confidenceChanged(double);

public slots:

  virtual void enableCrossCheck(bool enable = true) = 0;
  virtual void setRatio(double ratio) = 0;
  virtual void setDistance(double distance) = 0;
  //virtual void setMaxMatches(int maxMatches) = 0;
  virtual void setMaxError(double error) = 0;
  virtual void setConfidence(double confidence) = 0;
};


class GRAPHOS_EXPORT FeatureMatchingWidgetImp
  : public FeatureMatchingWidget
{

  Q_OBJECT

public:

  FeatureMatchingWidgetImp(QWidget *parent = nullptr);
  ~FeatureMatchingWidgetImp() override;

// FeatureMatchingWidget interface

public:

  bool crossCheck() const override;
  double ratio() const override;
  double distance() const override;
  //int maxMatches() const override;
  double maxError() const override;
  double confidence() const override;

public slots:

  void enableCrossCheck(bool enable) override;
  void setRatio(double ratio) override;
  void setDistance(double distance) override;
  //void setMaxMatches(int maxMatches) override;
  void setMaxError(double error) override;
  void setConfidence(double confidence) override;

// IWidgetView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGroupBox *mGroupBox;
  QCheckBox *mCheckBoxCrossCheck;
  QLabel *mLabelRatio;
  QDoubleSpinBox *mDoubleSpinBoxRatio;
  QLabel *mLabelDistance;
  QDoubleSpinBox *mDoubleSpinBoxDistance;
  QLabel *mLabelMaxError;
  QDoubleSpinBox *mDoubleSpinBoxMaxError;
  QLabel *mLabelConfidence;
  QDoubleSpinBox *mDoubleSpinBoxConfidence;

};

} // namespace graphos

#endif // FEATUREMATCHINGWIDGET_H
