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

#ifndef GRAPHOS_FEATURE_VIEWER_SETTINGS_WIDGET_H
#define GRAPHOS_FEATURE_VIEWER_SETTINGS_WIDGET_H

#include "graphos/widgets/GraphosWidget.h"

class QLineEdit;
class QLabel;
class QSpinBox;
class QPushButton;
class QListWidget;
class QGroupBox;

namespace graphos
{


class FeatureViewerSettingsWidget
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  FeatureViewerSettingsWidget(QWidget *parent = nullptr);
  virtual ~FeatureViewerSettingsWidget() = default;
  
  QString backgroundColor() const;
  int markerType() const;
  int markerSize() const;
  int markerWidth() const;
  QString markerColor() const;
  int selectedMarkerWidth() const;
  QString selectedMarkerColor() const;
  
public slots:

  void setBackgroundColor(const QString &color);
  void setMarkerType(int type);
  void setMarkerSize(int size);
  void setMarkerWidth(int width);
  void setMarkerColor(const QString &color);
  void setSelectedMarkerWidth(int width);
  void setSelectedMarkerColor(const QString &color);

signals:
  
  void backgroundColorChange(QString);
  void markerTypeChange(int);
  void markerSizeChange(int);
  void markerWidthChange(int);
  void markerColorChange(const QString &);
  void selectedMarkerWidthChange(int);
  void selectedMarkerColorChange(const QString &);

public slots:

protected slots:

  void onPushButtonBackgroundColorClicked();
  void onPushButtonMarkerColorClicked();
  void onPushButtonSelectedMarkerColorClicked();
  
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

  QLabel *mLabelBackgroundColor;
  QLineEdit *mLineEditBackgroundColor;
  QPushButton *mPushButtonBackgroundColor;

  QGroupBox *mGroupBoxMVMarker;
  QLabel *mLabelMarkerType;
  QListWidget *mListWidgetMarkerType;
  QLabel *mLabelMarkerSize;
  QSpinBox *mSpinBoxMarkerSize;
  QLabel *mLabelMarkerWidth;
  QSpinBox *mSpinBoxMarkerWidth;
  QLabel *mLabelMarkerColor;
  QLineEdit *mLineEditMarkerColor;
  QPushButton *mPushButtonMarkerColor;

  QGroupBox *mGroupBoxSelectMarker;
  QLabel *mLabelSelectMarkerWidth;
  QSpinBox *mSpinBoxSelectMarkerWidth;
  QLabel *mLabelSelectMarkerColor;
  QLineEdit *mLineEditSelectMarkerColor;
  QPushButton *mPushButtonSelectMarkerColor;

  
};


} // namespace graphos

#endif // GRAPHOS_FEATURE_VIEWER_SETTINGS_WIDGET_H
