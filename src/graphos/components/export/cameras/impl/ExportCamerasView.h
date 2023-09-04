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

#ifndef GRAPHOS_EXPORT_CAMERAS_VIEW_H
#define GRAPHOS_EXPORT_CAMERAS_VIEW_H

#include "graphos/components/export/cameras/ExportCamerasView.h"

class QLabel;
class QComboBox;
class QDialogButtonBox;
class QGridLayout;

namespace graphos
{

class ExportCamerasViewImp
  : public ExportCamerasView
{

  Q_OBJECT

public:

  ExportCamerasViewImp(QWidget *parent = nullptr);
  ~ExportCamerasViewImp() override;

// ExportCamerasView interface

public:

  void addFormatWidget(QWidget *formatWidget) override;
  QString format() const override;

public slots:

  void setCurrentFormat(const QString &format) override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QLabel *mLabelFormat;
  QComboBox *mComboBoxFormat;
  QGridLayout *mGridLayoutFormat;
  QDialogButtonBox *mButtonBox;

};

} // namespace graphos

#endif // GRAPHOS_EXPORT_ORIENTATIONS_VIEW_H
