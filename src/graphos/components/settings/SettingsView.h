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

#ifndef GRAPHOS_SETTINGS_VIEW_INTERFACE_H
#define GRAPHOS_SETTINGS_VIEW_INTERFACE_H

#include "graphos/core/mvp.h"

namespace graphos
{


class SettingsView
  : public DialogView
{
  Q_OBJECT

public:

  SettingsView(QWidget *parent) : DialogView(parent) {}
  virtual ~SettingsView() override = default;

public:

  virtual QString activeLanguage() const = 0;
  virtual int historyMaxSize() const = 0;
  virtual QString imageViewerBGColor() const = 0;
  virtual bool useCuda() const = 0;
  
public slots:

  virtual void setPage(int page) = 0;
  virtual void setLanguages(const QStringList &languages) = 0;
  virtual void setActiveLanguage(const QString &language) = 0;
  virtual void setHistoryMaxSize(int size) = 0;
  virtual void setImageViewerBGcolor(const QString &color) = 0;
  virtual void setUseCuda(bool active) = 0;
  virtual void setCudaEnabled(bool enabled) = 0;
  virtual void setUnsavedChanges(bool unsaveChanges) = 0;
  virtual void addWidget(QWidget *widget) = 0;

signals:

  void languageChange(QString);
  void historyMaxSizeChange(int);
  void imageViewerBGColorChange(QString);
  void useCudaChange(bool);
  void applyChanges();

};

} // namespace graphos

#endif // GRAPHOS_SETTINGS_VIEW_INTERFACE_H
