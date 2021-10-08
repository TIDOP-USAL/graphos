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

#ifndef GRAPHOS_DTM_INVDIST_WIDGET_H
#define GRAPHOS_DTM_INVDIST_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;

namespace graphos
{

class DtmInvDistWidget
  : public GraphosWidgetView
{

  Q_OBJECT

public:

  DtmInvDistWidget(QWidget *parent = nullptr) : GraphosWidgetView(parent){}
  ~DtmInvDistWidget() override = default;

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
   * \brief he first radius (X axis if rotation angle is 0) of search ellipse.
   * Set this parameter to zero to use whole point array. Default is 0.0.
   * \return
   */
  virtual double radius1() const = 0;

  /*!
   * \brief The second radius (Y axis if rotation angle is 0) of search ellipse.
   * Set this parameter to zero to use whole point array. Default is 0.0.
   * \return
   */
  virtual double radius2() const = 0;

  /*!
   * \brief Angle of search ellipse rotation in degrees (counter clockwise, default 0.0)
   * \return
   */
  virtual double angle() const = 0;

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
  void radius1Change(double);
  void radius2Change(double);
  void angleChange(double);
  void maxPointsChange(int);
  void minPointsChange(int);

public slots:

  virtual void setPower(double power) = 0;
  virtual void setSmoothing(double smoothing) = 0;
  virtual void setRadius1(double radius1) = 0;
  virtual void setRadius2(double radius2) = 0;
  virtual void setAngle(double angle) = 0;
  virtual void setMaxPoints(int maxPoints) = 0;
  virtual void setMinPoints(int minPoints) = 0;

};


class DtmInvDistWidgetImp
  : public DtmInvDistWidget
{

  Q_OBJECT

public:

  DtmInvDistWidgetImp(QWidget *parent = nullptr);
  ~DtmInvDistWidgetImp() override;

// GraphosWidgetView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:
  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// DtmInvDistWidget interface

public:

  double power() const override;
  double smoothing() const override;
  double radius1() const override;
  double radius2() const override;
  double angle() const override;
  int maxPoints() const override;
  int minPoints() const override;

public slots:

  void setPower(double power) override;
  void setSmoothing(double smoothing) override;
  void setRadius1(double radius1) override;
  void setRadius2(double radius2) override;
  void setAngle(double angle) override;
  void setMaxPoints(int maxPoints) override;
  void setMinPoints(int minPoints) override;

protected:

  QGroupBox *mGroupBox;
  QLabel *mLabelPower;
  QDoubleSpinBox *mDoubleSpinBoxPower;
  QLabel *mLabelSmoothing;
  QDoubleSpinBox *mDoubleSpinBoxSmoothing;
  QLabel *mLabelRadius1;
  QDoubleSpinBox *mDoubleSpinBoxRadius1;
  QLabel *mLabelRadius2;
  QDoubleSpinBox *mDoubleSpinBoxRadius2;
  QLabel *mLabelAngle;
  QDoubleSpinBox *mDoubleSpinBoxAngle;
  QLabel *mLabelMaxPoints;
  QSpinBox *mSpinBoxMaxPoints;
  QLabel *mLabelMinPoints;
  QSpinBox *mSpinBoxMinPoints;

};


} // namespace graphos

#endif // GRAPHOS_DTM_INVDIST_WIDGET_H
