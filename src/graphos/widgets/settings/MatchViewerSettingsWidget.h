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
  
  QString matchesViewerBGColor() const;
  int matchesViewerMarkerType() const;
  int matchesViewerMarkerSize() const;
  int matchesViewerMarkerWidth() const;
  QString matchesViewerMarkerColor() const;
  int selectMatchesViewerMarkerWidth() const;
  QString selectMatchesViewerMarkerColor() const;
  QString matchesViewerLineColor() const;
  int matchesViewerLineWidth() const;
  
public slots:
  
  void setMatchesViewerBGColor(const QString &color);
  void setMatchesViewerMarkerType(int type);
  void setMatchesViewerMarkerSize(int size);
  void setMatchesViewerMarkerWidth(int width);
  void setMatchesViewerMarkerColor(const QString &color);
  void setSelectMatchesViewerMarkerWidth(int width);
  void setSelectMatchesViewerMarkerColor(const QString &color);
  void setMatchesViewerLineColor(const QString &color);
  void setMatchesViewerLineWidth(int width);
  
signals:
  
  void matchesViewerBGColorChange(const QString &);
  void matchesViewerMarkerTypeChange(int);
  void matchesViewerMarkerSizeChange(int);
  void matchesViewerMarkerWidthChange(int);
  void matchesViewerMarkerColorChange(const QString &);
  void matchesViewerLineWidthChange(int);
  void matchesViewerLineColorChange(const QString &);
  void selectMatchesViewerMarkerWidthChange(int);
  void selectMatchesViewerMarkerColorChange(const QString &);

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
