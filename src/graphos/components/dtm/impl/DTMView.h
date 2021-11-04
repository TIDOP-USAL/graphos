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

#ifndef GRAPHOS_DTM_VIEW_H
#define GRAPHOS_DTM_VIEW_H

#include "graphos/components/dtm/DTMView.h"

class QGridLayout;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QDoubleSpinBox;
class QRadioButton;

namespace graphos
{

class DtmViewImp
  : public DtmView
{

  Q_OBJECT

public:

  DtmViewImp(QWidget *parent = nullptr);
  ~DtmViewImp() override;

//private slots:
//
//  void onMdtSelect();
//  void onMdsSelect();

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// DtmView interface

public:

  double gsd() const override;
  void addDtmMethod(QWidget *method) override;
  QString currentDtmMethod() const override;
  bool isDSM() const override;

public slots:

  void setGSD(double gsd) override;
  void setCurrentDtmMethod(const QString &method) override;
  void setDSM(bool active) override;

private:

  QGridLayout *mGridLayoutDtmMethod;
  QLabel *mLabelGSD;
  QDoubleSpinBox *mDoubleSpinBoxGSD;
  QRadioButton *mRadioButtonMdt;
  QRadioButton *mRadioButtonMds;
  QLabel *mLabelDtmMethod;
  QComboBox *mComboBoxDtmMethod;
  QDialogButtonBox *mButtonBox;
};

} // namespace graphos

#endif // GRAPHOS_DTM_VIEW_H
