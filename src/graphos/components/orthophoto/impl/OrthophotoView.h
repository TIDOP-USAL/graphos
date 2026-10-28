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

#pragma once

#include "graphos/components/orthophoto/OrthophotoView.h"

class QDialogButtonBox;
class QLabel;
class QDoubleSpinBox;
class QComboBox;
class QLineEdit;
class QGroupBox;
class QGridLayout;

namespace graphos
{

class OrthophotoViewImp
  : public OrthophotoView
{

    Q_OBJECT

public:

    OrthophotoViewImp(QWidget *parent = nullptr);
    ~OrthophotoViewImp() override;

// OrthophotoView

public:

    auto gsd() const -> double override;
    auto maxTiltAngle() const -> double override;
    auto crs() const -> QString override;
    auto interpolation() const -> QString override;
    auto orthomosaicMethod() const -> QString override;

public slots:

    void setGsd(double gsd) override;
    void setMaxTiltAngle(double maxTiltAngle) override;
    void setCrs(const QString &crs) override;
    void setInterpolation(const QString &interpolation) override;
    void setOrthomosaicMethod(const QString &method) override;
    void addOrthomosaicMethodWidget(QWidget *orthomosaic) override;

// DialogView

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

protected:

    QLabel *mLabelCRS;
    QLineEdit *mLineEditCRS;
    QPushButton *mPushButtonCRS;
    QGroupBox *mGroupOrthoimages;
    QLabel *mLabelGSD;
    QDoubleSpinBox *mDoubleSpinBoxGSD;
    QLabel *mLabelmaxTiltAngle;
    QDoubleSpinBox *mDoubleSpinBoxMaxTiltAngle;
    QLabel *mLabelInterpolation;
    QComboBox *mComboBoxInterpolation;
    QGroupBox *mGroupOrthomosaic;
    QGridLayout *mGridLayoutOrthomosaicWidget;
    QLabel *mLabelMethod;
    QComboBox *mComboBoxMethod;
    QDialogButtonBox *mButtonBox;

};

} // namespace graphos
