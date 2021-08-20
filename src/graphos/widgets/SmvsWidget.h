#ifndef GRAPHOS_SMVS_WIDGET_H
#define GRAPHOS_SMVS_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{

/*!
 * \brief Shading-Aware Multi-View Stereo Widget Interface
 */
class GRAPHOS_EXPORT SmvsWidget
  : public IWidgetView
{
  Q_OBJECT

public:

  SmvsWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~SmvsWidget() = default;

  virtual int inputImageScale() const = 0;
  virtual int outputDepthScale() const  = 0;
  virtual bool shadingBasedOptimization() const = 0;
  virtual bool semiGlobalMatching() const = 0;
  virtual double surfaceSmoothingFactor() const = 0;

signals:

  void inputImageScaleChanged(int);
  void outputDepthScaleChanged(int);
  void shadingBasedOptimizationChanged(bool);
  void semiGlobalMatchingChanged(bool);
  void surfaceSmoothingFactorChanged(double);

public slots:

  virtual void setInputImageScale(int inputImageScale) = 0;
  virtual void setOutputDepthScale(int outputDepthScale)  = 0;
  virtual void setShadingBasedOptimization(bool shadingBasedOptimization) = 0;
  virtual void setSemiGlobalMatching(bool semiGlobalMatching) = 0;
  virtual void setSurfaceSmoothingFactor(double surfaceSmoothingFactor) = 0;
};


class GRAPHOS_EXPORT SmvsWidgetImp
  : public SmvsWidget
{
  Q_OBJECT

public:

  SmvsWidgetImp(QWidget *parent = nullptr);
  ~SmvsWidgetImp() override;

// ISmvsWidget interface

public:

  int inputImageScale() const override;
  int outputDepthScale() const  override;
  bool shadingBasedOptimization() const override;
  bool semiGlobalMatching() const override;
  double surfaceSmoothingFactor() const override;

public slots:

  void setInputImageScale(int inputImageScale) override;
  void setOutputDepthScale(int outputDepthScale)  override;
  void setShadingBasedOptimization(bool shadingBasedOptimization) override;
  void setSemiGlobalMatching(bool semiGlobalMatching) override;
  void setSurfaceSmoothingFactor(double surfaceSmoothingFactor) override;

// GraphosWidget interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

protected:
  
  QGroupBox *mGroupBox;
  QLabel *mLabelInputImageScale;
  QSpinBox *mInputImageScale;
  QLabel *mLabelOutputDepthScale;
  QSpinBox *mOutputDepthScale;
  QCheckBox *mShadingBasedOptimization;
  QCheckBox *mSemiGlobalMatching;
  QLabel *mLabelSurfaceSmoothingFactor;
  QDoubleSpinBox *mSurfaceSmoothingFactor;

};

} // namespace graphos

#endif // GRAPHOS_SMVS_WIDGET_H
