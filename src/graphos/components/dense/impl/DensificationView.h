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

#ifndef GRAPHOS_DENSE_VIEW_H
#define GRAPHOS_DENSE_VIEW_H

#include "graphos/components/dense/DensificationView.h"

class QGridLayout;
class QDialogButtonBox;
class QComboBox;
class QLabel;

namespace graphos
{

class DensificationViewImp
  : public DensificationView
{
  Q_OBJECT

public:

  explicit DensificationViewImp(QWidget *parent = nullptr);
  ~DensificationViewImp() override;

// DensificationView interface

public:

  QString currentDensificationMethod() const override;

public slots:

  void addDensification(QWidget *densification) override;
  void setCurrentDensificationMethod(const QString &densificationMethod) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

private:

  QGridLayout *mGridLayout;
  QGridLayout *mGridLayoutDensification;
  QLabel *mLabelDensification;
  QComboBox *mComboBoxDensification;
  QDialogButtonBox *mButtonBox;

};

} // End namespace graphos


#endif // GRAPHOS_DENSE_VIEW_H
