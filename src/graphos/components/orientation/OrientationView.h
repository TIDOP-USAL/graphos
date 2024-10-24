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

#ifndef GRAPHOS_ORIENTATION_VIEW_INTERFACE_H
#define GRAPHOS_ORIENTATION_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"


namespace graphos
{

class OrientationView
  : public DialogView
{

    Q_OBJECT

public:

    OrientationView(QWidget *parent = nullptr) : DialogView(parent) {}
    ~OrientationView() override = default;

    virtual auto fixCalibration() const -> bool = 0;
    virtual auto isEnabledCalibration() const -> bool = 0;
    virtual auto absoluteOrientation() const -> bool = 0;
    virtual auto isEnabledAbsoluteOrientation() const -> bool = 0;
    virtual auto fixPoses() const -> bool = 0;
    virtual auto isEnabledPoses() const -> bool = 0;

public slots:

    virtual void setCalibration(bool active) = 0;
    virtual void enabledCalibration(bool enabled) = 0;
    virtual void setAbsoluteOrientation(bool active) = 0;
    virtual void enabledAbsoluteOrientation(bool enabled) = 0;
    virtual void setPoses(bool active) = 0;
    virtual void enabledPoses(bool enabled) = 0;

signals:

    void run();
    void calibrationChange(bool);
    void absoluteOrientationChange(bool);
    void posesChange(bool);
};

} // namespace graphos

#endif // GRAPHOS_FEATURE_EXTRACTOR_VIEW_INTERFACE_H
