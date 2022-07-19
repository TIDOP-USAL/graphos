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

#ifndef GRAPHOS_MATCH_VIEWER_SETTINGS_WIDGET_H
#define GRAPHOS_MATCH_VIEWER_SETTINGS_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QLineEdit;
class QLabel;
class QSpinBox;
class QPushButton;
class QListWidget;

namespace graphos
{


class MatchViewerSettingsWidget
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  MatchViewerSettingsWidget(QWidget *parent = nullptr);
  virtual ~MatchViewerSettingsWidget() = default;
  
  QString backgroundColor() const;
  int markerType() const;
  int markerSize() const;
  int markerWidth() const;
  QString markerColor() const;
  int selectedMarkerWidth() const;
  QString selectedMarkerColor() const;
  QString lineColor() const;
  int lineWidth() const;
  
public slots:
  
  void setBackgroundColor(const QString &color);
  void setMarkerType(int type);
  void setMarkerSize(int size);
  void setMarkerWidth(int width);
  void setMarkerColor(const QString &color);
  void setSelectedMarkerWidth(int width);
  void setSelectedMarkerColor(const QString &color);
  void setLineColor(const QString &color);
  void setLineWidth(int width);
  
signals:
  
  void backgroundColorChange(const QString &);
  void markerTypeChange(int);
  void markerSizeChange(int);
  void markerWidthChange(int);
  void markerColorChange(const QString &);
  void lineWidthChange(int);
  void lineColorChange(const QString &);
  void selectedMarkerWidthChange(int);
  void selectedMarkerColorChange(const QString &);

public slots:

protected slots:

  void onPushButtonMatchViewerBGColorClicked();
  void onPushButtonMatchViewerMarkerColorClicked();
  void onPushButtonSelectMatchViewerMarkerColorClicked();
  void onPushButtonMatchViewerLineColorClicked();

// GraphosWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  void initUI() override;
  void initSignalAndSlots() override;

protected:

  QLineEdit *mLineEditMatchesViewerBGColor;
  QPushButton *mPushButtonMatchesViewerBGColor;
  QLineEdit *mLineEditMatchesViewerMarkerColor;
  QPushButton *mPushButtonMatchesViewerMarkerColor;
  QSpinBox *mSpinBoxMatchesViewerMarkerSize;
  QSpinBox *mSpinBoxMatchesViewerMarkerWidth;
  QListWidget *mListWidgetMatchesViewerMarkerType;
  QLineEdit *mLineEditMatchesViewerLineColor;
  QPushButton *mPushButtonMatchesViewerLineColor;
  QSpinBox *mSpinBoxMatchesViewerLineWidth;
  QSpinBox *mSpinBoxSelectMatchesViewerMarkerWidth;
  QLineEdit *mLineEditSelectMatchesViewerMarkerColor;
  QPushButton *mPushButtonSelectMatchesViewerMarkerColor;
  
};


} // namespace graphos

#endif // GRAPHOS_MATCH_VIEWER_SETTINGS_WIDGET_H
