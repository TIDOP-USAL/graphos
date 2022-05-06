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

#ifndef GRAPHOS_CORE_SETTINGS_H
#define GRAPHOS_CORE_SETTINGS_H

#include "graphos/graphos_global.h"

#include <QStringList>
#include <QSize>

class QSettings;

namespace graphos
{

/*!
 * \brief Settings interface
 */
class Settings
{

public:

  Settings() {}
  virtual ~Settings() = default;

  /*!
   * \brief Current language
   * \return Current language
   */
  virtual QString language() const = 0;

  /*!
   * \brief Set the language
   * \param[in] language GUI language
   */
  virtual void setLanguage(const QString &language) = 0;

  /*!
   * \brief Recent history projects
   * \return List of recent projects
   */
  //virtual QStringList history() const = 0;

  /*!
   * \brief Add a project to the history
   * \param[in] project Project path
   */
  //virtual void addToHistory(const QString &project) = 0;

  /*!
   * \brief Clear the history of recent projects
   */
  //virtual void clearHistory() = 0;

  /*!
   * \brief Maximum history size
   * \return Maximum history size
   */
  virtual int historyMaxSize() const = 0;

  /*!
   * \brief Set the size number of history items
   * \param[in] maxSize History size
   */
  virtual void setHistoryMaxSize(int maxSize) = 0;

  /*!
   * \brief Image viewer Background color
   * \return Hex value
   */
  virtual QString imageViewerBGcolor() const = 0;

  /*!
   * \brief Set Image Viewer background color
   * \param bgColor Hex value
   */
  virtual void setImageViewerBGcolor(const QString &bgColor) = 0;

  virtual QString keypointsViewerBGColor() const = 0;
  virtual void setKeypointsViewerBGColor(const QString &color) = 0;
  virtual int keypointsViewerMarkerType() const = 0;
  virtual void setKeypointsViewerMarkerType(int type) = 0;
  virtual int keypointsViewerMarkerSize() const = 0;
  virtual void setKeypointsViewerMarkerSize(int size) = 0;
  virtual int keypointsViewerMarkerWidth() const = 0;
  virtual void setKeypointsViewerMarkerWidth(int width) = 0;
  virtual QString keypointsViewerMarkerColor() const = 0;
  virtual void setKeypointsViewerMarkerColor(const QString &color) = 0;
  virtual int keypointsViewerSelectMarkerWidth() const = 0;
  virtual void setKeypointsViewerSelectMarkerWidth(int width) = 0;
  virtual QString keypointsViewerSelectMarkerColor() const = 0;
  virtual void setKeypointsViewerSelectMarkerColor(const QString &color) = 0;

  virtual QString matchesViewerBGColor() const = 0;
  virtual void setMatchesViewerBGColor(const QString &color) = 0;
  virtual int matchesViewerMarkerType() const = 0;
  virtual void setMatchesViewerMarkerType(int type) = 0;
  virtual int matchesViewerMarkerSize() const = 0;
  virtual void setMatchesViewerMarkerSize(int size) = 0;
  virtual int matchesViewerMarkerWidth() const = 0;
  virtual void setMatchesViewerMarkerWidth(int width) = 0;
  virtual QString matchesViewerMarkerColor() const = 0;
  virtual void setMatchesViewerMarkerColor(const QString &color) = 0;
  virtual int matchesViewerSelectMarkerWidth() const = 0;
  virtual void setMatchesViewerSelectMarkerWidth(int width) = 0;
  virtual QString matchesViewerSelectMarkerColor() const = 0;
  virtual void setMatchesViewerSelectMarkerColor(const QString &color) = 0;
  virtual QString matchesViewerLineColor() const = 0;
  virtual void setMatchesViewerLineColor(const QString &color) = 0;
  virtual int matchesViewerLineWidth() const = 0;
  virtual void setMatchesViewerLineWidth(int width) = 0;

  virtual bool useCuda() const = 0;
  virtual void setUseCuda(bool active) = 0;

  /*!
   * \brief Recupera la configuración por defecto
   */
  virtual void reset() = 0;

};


/*----------------------------------------------------------------*/


class SettingsController
{

public:

  SettingsController() {}
  virtual ~SettingsController() = default;

  /*!
   * \brief read
   * \return
   */
  virtual void read(Settings &settings) = 0;

  /*!
   * \brief write
   * \return
   */
  virtual void write(const Settings &settings) = 0;

  //virtual void writeHistory(const Settings &settings) = 0;
};



/*----------------------------------------------------------------*/


/*!
 * \brief The Settings class
 */
class SettingsImp
  : public Settings
{

public:

  SettingsImp();
  ~SettingsImp() override;

// Settings interface

public:

  QString language() const override;
  void setLanguage(const QString &language) override;

  //QStringList history() const override;
  //void addToHistory(const QString &project) override;
  //void clearHistory() override;
  int historyMaxSize() const override;
  void setHistoryMaxSize(int maxSize) override;

  QString imageViewerBGcolor() const override;
  void setImageViewerBGcolor(const QString &bgColor) override;

  QString keypointsViewerBGColor() const override;
  void setKeypointsViewerBGColor(const QString &color) override;
  int keypointsViewerMarkerType() const override;
  void setKeypointsViewerMarkerType(int type) override;
  int keypointsViewerMarkerSize() const override;
  void setKeypointsViewerMarkerSize(int size) override;
  int keypointsViewerMarkerWidth() const override;
  void setKeypointsViewerMarkerWidth(int width) override;
  QString keypointsViewerMarkerColor() const override;
  void setKeypointsViewerMarkerColor(const QString &color) override;
  int keypointsViewerSelectMarkerWidth() const override;
  void setKeypointsViewerSelectMarkerWidth(int width) override;
  QString keypointsViewerSelectMarkerColor() const override;
  void setKeypointsViewerSelectMarkerColor(const QString &color) override;

  QString matchesViewerBGColor() const override;
  void setMatchesViewerBGColor(const QString &color) override;
  int matchesViewerMarkerType() const override;
  void setMatchesViewerMarkerType(int type) override;
  int matchesViewerMarkerSize() const override;
  void setMatchesViewerMarkerSize(int size) override;
  int matchesViewerMarkerWidth() const override;
  void setMatchesViewerMarkerWidth(int width) override;
  QString matchesViewerMarkerColor() const override;
  void setMatchesViewerMarkerColor(const QString &color) override;
  int matchesViewerSelectMarkerWidth() const override;
  void setMatchesViewerSelectMarkerWidth(int width) override;
  QString matchesViewerSelectMarkerColor() const override;
  void setMatchesViewerSelectMarkerColor(const QString &color) override;
  QString matchesViewerLineColor() const override;
  void setMatchesViewerLineColor(const QString &color) override;
  int matchesViewerLineWidth() const override;
  void setMatchesViewerLineWidth(int width) override;

  bool useCuda() const override;
  void setUseCuda(bool active) override;

  void reset() override;

protected:

  QString mLanguage;
  int mHistoyMaxSize;
  //QStringList mHistory;
  QString mImageViewerBGcolor;
  QString mKeypointViewerBGColor;
  int mKeypointsViewerMarkerType;
  int mKeypointViewerMarkerSize;
  int mKeypointViewerMarkerWidth;
  QString mKeypointViewerMarkerColor;
  int mKeypointViewerSelectMarkerWidth;
  QString mKeypointViewerSelectMarkerColor;
  QString mMatchesViewerBGColor;
  int mMatchesViewerMarkerType;
  QString mMatchesViewerMarkerColor;
  int mMatchesViewerMarkerSize;
  int mMatchesViewerMarkerWidth;
  int mMatchesViewerSelectMarkerWidth;
  QString mMatchesViewerSelectMarkerColor;
  QString mMatchesViewerLineColor;
  int mMatchesViewerLineWidth;
  bool mUseCuda;

};


/*----------------------------------------------------------------*/


class SettingsControllerImp
  : public SettingsController
{

public:

  explicit SettingsControllerImp();
  ~SettingsControllerImp() override;

// SettingsController interface

public:

  void read(Settings &settings) override;
  void write(const Settings &settings) override;
  //void writeHistory(const Settings &settings) override;

protected:

  QSettings *mSettingsController;

};


} // namespace graphos

#endif // GRAPHOS_CORE_SETTINGS_H
