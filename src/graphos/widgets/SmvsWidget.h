/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/
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
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  SmvsWidget(QWidget *parent = nullptr);
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

// GraphosWidgetView interface

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
