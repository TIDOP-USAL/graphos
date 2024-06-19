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

#ifndef GRAPHOS_DTM_VIEW_H
#define GRAPHOS_DTM_VIEW_H

#include "graphos/components/dtm/DTMView.h"

class QGridLayout;
class QDialogButtonBox;
class QLabel;
class QDoubleSpinBox;
class QCheckBox;

namespace graphos
{

class DtmViewImp
  : public DtmView
{

    Q_OBJECT

public:

    DtmViewImp(QWidget *parent = nullptr);
    ~DtmViewImp() override;

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

    auto gsd() const -> double override;
    auto isMdsEnable() const -> bool override;
    auto isMdtEnable() const -> bool override;

public slots:

    void setGsd(double gsd) override;
    void enableMds(bool enable = true) override;
    void enableMdt(bool enable = true) override;

private:

    QGridLayout *mGridLayoutDtmMethod;
    QCheckBox *mCheckBoxMDS;
    QCheckBox *mCheckBoxMDT;
    QLabel *mLabelGSD;
    QDoubleSpinBox *mDoubleSpinBoxGSD;
    QDialogButtonBox *mButtonBox;
};

} // namespace graphos

#endif // GRAPHOS_DTM_VIEW_H
