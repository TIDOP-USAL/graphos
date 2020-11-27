#ifndef INSPECTOR_DTM_INVDISTNN_WIDGET_H
#define INSPECTOR_DTM_INVDISTNN_WIDGET_H

#include "inspector/widgets/InspectorWidget.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;

namespace inspector
{

class DtmInvDistNNWidget
  : public IWidgetView
{

  Q_OBJECT

public:

  DtmInvDistNNWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~DtmInvDistNNWidget() override = default;

  /*!
   * \brief Weighting power (default 2.0)
   * \return
   */
  virtual double power() const = 0;

  /*!
   * \brief Smoothing parameter (default 0.0).
   * \return
   */
  virtual double smoothing() const = 0;

  /*!
   * \brief The radius of the search circle, which should be non-zero. Default is 1.0.
   * \return
   */
  virtual double radius() const = 0;

  /*!
   * \brief Maximum number of data points to use.
   * Do not search for more points than this number. This is only used if search
   * ellipse is set (both radii are non-zero). Zero means that all found points
   * should be used. Default is 0.
   * \return
   */
  virtual int maxPoints() const = 0;

  /*!
   * \brief Minimum number of data points to use.
   * If less amount of points found the grid node considered empty and will
   * be filled with NODATA marker. This is only used if search ellipse
   * is set (both radii are non-zero). Default is 0.
   * \return
   */
  virtual int minPoints() const = 0;

signals:

  void powerChange(double);
  void smoothingChange(double);
  void radiusChange(double);
  void maxPointsChange(int);
  void minPointsChange(int);

public slots:

  virtual void setPower(double power) = 0;
  virtual void setSmoothing(double smoothing) = 0;
  virtual void setRadius(double radius) = 0;
  virtual void setMaxPoints(int maxPoints) = 0;
  virtual void setMinPoints(int minPoints) = 0;

};


class DtmInvDistNNWidgetImp
  : public DtmInvDistNNWidget
{

  Q_OBJECT

public:

  DtmInvDistNNWidgetImp(QWidget *parent = nullptr);
  ~DtmInvDistNNWidgetImp() override;

// IWidgetView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:
  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// DtmInvDistNNWidget interface

public:

  double power() const override;
  double smoothing() const override;
  double radius() const override;
  int maxPoints() const override;
  int minPoints() const override;

public slots:

  void setPower(double power) override;
  void setSmoothing(double smoothing) override;
  void setRadius(double radius) override;
  void setMaxPoints(int maxPoints) override;
  void setMinPoints(int minPoints) override;

protected:

  QGroupBox *mGroupBox;
  QLabel *mLabelPower;
  QDoubleSpinBox *mDoubleSpinBoxPower;
  QLabel *mLabelSmoothing;
  QDoubleSpinBox *mDoubleSpinBoxSmoothing;
  QLabel *mLabelRadius;
  QDoubleSpinBox *mDoubleSpinBoxRadius;
  QLabel *mLabelMaxPoints;
  QSpinBox *mSpinBoxMaxPoints;
  QLabel *mLabelMinPoints;
  QSpinBox *mSpinBoxMinPoints;

};


} // namespace inspector

#endif // INSPECTOR_DTM_INVDISTNN_WIDGET_H
