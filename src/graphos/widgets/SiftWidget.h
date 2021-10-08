/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_SIFT_WIDGET_H
#define GRAPHOS_SIFT_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{

/*!
 * \brief Interface SIFT Widget
 */
class GRAPHOS_EXPORT SiftWidget
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  SiftWidget(QWidget *parent = nullptr);
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
  virtual double contrastThreshold() const = 0;

  virtual bool constrastThresholdAuto() const = 0;

  /*!
   * \brief edgeThreshold
   * \return
   */
  virtual double edgeThreshold() const = 0;

  /*!
   * \brief sigma
   * \return
   */
  virtual double sigma() const = 0;

signals:

  void featuresNumberChange(int);
  void octaveLayersChange(int);
  void contrastThresholdAutoChange(bool);
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

  virtual void setContrastThresholdAuto(bool active) = 0;

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


class GRAPHOS_EXPORT SiftWidgetImp
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
  double contrastThreshold() const override;
  bool constrastThresholdAuto() const override;
  double edgeThreshold() const override;
  double sigma() const override;

public slots:

  void setFeaturesNumber(int featuresNumber) override;
  void setOctaveLayers(int octaveLayers) override;
  void setContrastThreshold(double contrastThreshold) override;
  void setContrastThresholdAuto(bool active) override;
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
  QCheckBox *mContrastThresholdAuto;
  QLabel *mLabelContrastThreshold;
  QDoubleSpinBox *mContrastThreshold;
  QLabel *mLabelEdgeThreshold;
  QDoubleSpinBox *mEdgeThreshold;
  QLabel *mLabelSigma;
  QDoubleSpinBox *mSigma;

};

} // namespace graphos

#endif // GRAPHOS_SIFT_WIDGET_H
