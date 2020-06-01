#ifndef INSPECTOR_SIFT_WIDGET_H
#define INSPECTOR_SIFT_WIDGET_H

#include "inspector/widgets/InspectorWidget.h"


class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;

namespace inspector
{

/*!
 * \brief Interface SIFT Widget
 */
class INSPECTOR_EXPORT SiftWidget
  : public IWidgetView
{
  Q_OBJECT

public:

  SiftWidget(QWidget *parent = nullptr) : IWidgetView(parent){}
  ~SiftWidget() override = default;

  /*!
   * \brief featuresNumber
   * \return
   */
  virtual int featuresNumber() const = 0;

  /*!
   * \brief octaveLayers
   * \return
   */
  virtual int octaveLayers() const = 0;

  /*!
   * \brief contrastThreshold
   * \return
   */
  virtual double contrastThreshold() = 0;

  /*!
   * \brief edgeThreshold
   * \return
   */
  virtual double edgeThreshold() = 0;

  /*!
   * \brief sigma
   * \return
   */
  virtual double sigma() = 0;

signals:

  void featuresNumberChange(int);
  void octaveLayersChange(int);
  void contrastThresholdChange(double);
  void edgeThresholdChange(double);
  void sigmaChange(double);

public slots:

  /*!
   * \brief setFeaturesNumber
   * \param featuresNumber
   */
  virtual void setFeaturesNumber(int featuresNumber) = 0;

  /*!
   * \brief setOctaveLayers
   * \param octaveLayers
   */
  virtual void setOctaveLayers(int octaveLayers) = 0;

  /*!
   * \brief setContrastThreshold
   * \param contrastThreshold
   */
  virtual void setContrastThreshold(double contrastThreshold) = 0;

  /*!
   * \brief setEdgeThreshold
   * \param edgeThreshold
   */
  virtual void setEdgeThreshold(double edgeThreshold) = 0;

  /*!
   * \brief setSigma
   * \param sigma
   */
  virtual void setSigma(double sigma) = 0;

};


class INSPECTOR_EXPORT SiftWidgetImp
  : public SiftWidget
{
  Q_OBJECT

public:

  SiftWidgetImp(QWidget *parent = nullptr);
  ~SiftWidgetImp() override;

// ISiftWidget interface

public:

  int featuresNumber() const override;
  int octaveLayers() const override;
  double contrastThreshold() override;
  double edgeThreshold() override;
  double sigma() override;

public slots:

  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setEdgeThreshold(double edgeThreshold) override;
  void setSigma(double sigma) override;

// PhotoMatchWidget interface

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
  QLabel *mLabelFeaturesNumber;
  QSpinBox *mFeaturesNumber;
  QLabel *mLabelOctaveLayers;
  QSpinBox *mOctaveLayers;
  QLabel *mLabelContrastThreshold;
  QDoubleSpinBox *mContrastThreshold;
  QLabel *mLabelEdgeThreshold;
  QDoubleSpinBox *mEdgeThreshold;
  QLabel *mLabelSigma;
  QDoubleSpinBox *mSigma;

};

} // namespace inspector

#endif // INSPECTOR_SIFT_WIDGET_H
