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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/


#ifndef GRAPHOS_MESH_VIEW_H
#define GRAPHOS_MESH_VIEW_H

#include "graphos/components/mesh/MeshView.h"

class QDialogButtonBox;
class QLabel;
class QSpinBox;
class QLabel;
class QSpinBox;
class QLabel;
class QComboBox;
class QLabel;
class QSpinBox;
class QLabel;
class QSpinBox;


namespace graphos
{

class MeshViewImp
  : public MeshView
{

    Q_OBJECT

public:

    MeshViewImp(QWidget *parent = nullptr);
    ~MeshViewImp() override;

// MeshView

public:

    int depth() const override;
    int solveDepth() const override;
    QString boundaryType() const override;
    //int width() const override;
    //int fullDepth() const override;

public slots:

    void setDepth(int Depth) override;
    void setSolveDepth(int SolveDepth) override;
    void setBoundaryType(const QString &BoundaryType) override;
    //void setWidth(int width) override;
    //void setFullDepth(int FullDepth) override;

// DialogView

private:

    void initUI();
    void initSignalAndSlots();

public slots:

    void clear();

private slots:

    void update();
    void retranslate();

protected:

    QLabel *mLabelDepth;
    QSpinBox *mSpinBoxDepth;
    QLabel *mLabelSolveDepth;
    QSpinBox *mSpinBoxSolveDepth;
    QLabel *mLabelBoundaryType;
    QComboBox *mComboBoxBoundaryType;
    //QLabel *mLabelWidth;
    //QSpinBox *mSpinBoxWidth;
    //QLabel *mLabelFullDepth;
    //QSpinBox *mSpinBoxFullDepth;
    QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_MESH_VIEW_H
