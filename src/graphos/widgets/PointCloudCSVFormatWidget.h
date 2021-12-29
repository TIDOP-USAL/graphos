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

#ifndef GRAPHOS_POINT_CLOUD_CSV_FORMAT_WIDGET_H
#define GRAPHOS_POINT_CLOUD_CSV_FORMAT_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QRadioButton;

namespace graphos
{

class GRAPHOS_EXPORT PointCloudCSVFormatWidget
  : public GraphosWidgetView
{

  Q_OBJECT

public:

  PointCloudCSVFormatWidget(QWidget *parent = nullptr) : GraphosWidgetView(parent){}
  ~PointCloudCSVFormatWidget() override = default;

  virtual QString delimiter() const = 0;

signals:

  void delimiterChanged(QString);

public slots:

  virtual void setDelimiter(const QString &delimiter) = 0;

};

class GRAPHOS_EXPORT PointCloudCSVFormatWidgetImp
  : public PointCloudCSVFormatWidget
{

  Q_OBJECT

public:

  PointCloudCSVFormatWidgetImp(QWidget *parent = nullptr);
  ~PointCloudCSVFormatWidgetImp() override = default;

protected slots:

  void onDelimiterChanged();

// PointCloudCSVFormatWidget interface

public:

  QString delimiter() const override;

public slots:

  void setDelimiter(const QString &delimiter) override;

// GraphosWidgetView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGroupBox *mGroupBoxDelimiter;
  QRadioButton *mRadioButtonTab;
  QRadioButton *mRadioButtonComma;
  QRadioButton *mRadioButtonSpace;
  QRadioButton *mRadioButtonSemicolon;

};

} // namespace graphos


#endif // GRAPHOS_POINT_CLOUD_CSV_FORMAT_WIDGET_H
