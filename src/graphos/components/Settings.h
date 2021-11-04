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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_SETTINGS_INTERFACES_H
#define GRAPHOS_SETTINGS_INTERFACES_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

class SettingsModel
  : public Model
{

  Q_OBJECT

public:

  SettingsModel(QObject *parent = nullptr) : Model(parent) {}
  virtual ~SettingsModel() override = default;

  /*!
   * \brief List of available languages
   * \return
   */
  virtual QStringList languages() const = 0;

  /*!
   * \brief Current language
   * \return Current language
   */
  virtual QString language() const = 0;

  /*!
   * \brief Recent history projects
   * \return List of recent projects
   */
  virtual QStringList history() const = 0;

  /*!
   * \brief Maximum history size
   * \return Maximum history size
   */
  virtual int historyMaxSize() const = 0;

  /*!
   * \brief Image viewer Background color
   * \return Hex value
   */
  virtual QString imageViewerBGcolor() const = 0;

  virtual bool useCuda() const = 0;

  ///TODO: Sift Properties

  virtual QString keypointsViewerBGColor() const = 0;
  virtual int keypointsViewerMarkerType() const = 0;
  virtual int keypointsViewerMarkerSize() const = 0;
  virtual int keypointsViewerMarkerWidth() const = 0;
  virtual QString keypointsViewerMarkerColor() const = 0;
  virtual int keypointsViewerSelectMarkerWidth() const = 0;
  virtual QString keypointsViewerSelectMarkerColor() const = 0;

  virtual QString matchesViewerBGColor() const = 0;
  virtual int matchesViewerMarkerType() const = 0;
  virtual int matchesViewerMarkerSize() const = 0;
  virtual int matchesViewerMarkerWidth() const = 0;
  virtual QString matchesViewerMarkerColor() const = 0;
  virtual int matchesViewerSelectMarkerWidth() const = 0;
  virtual QString matchesViewerSelectMarkerColor() const = 0;
  virtual QString matchesViewerLineColor() const = 0;
  virtual int matchesViewerLineWidth() const = 0;


  /*!
   * \brief read
   * \return
   */
  virtual void read() = 0;

  /*!
   * \brief write
   * \return
   */
  virtual void write() = 0;

public slots:

  /*!
   * \brief Set the language
   * \param[in] language GUI language
   */
  virtual void setLanguage(const QString &language) = 0;

  /*!
   * \brief Add a project to the history
   * \param[in] project Project path
   */
  virtual void addToHistory(const QString &project) = 0;

  /*!
   * \brief Clear the history of recent projects
   */
  virtual void clearHistory() = 0;

  /*!
   * \brief Set the size number of history items
   * \param[in] maxSize History size
   */
  virtual void setHistoryMaxSize(int maxSize) = 0;

  /*!
   * \brief Set Image Viewer background color
   * \param bgColor Hex value
   */
  virtual void setImageViewerBGcolor(const QString &bgColor) = 0;

  virtual void setUseCuda(bool active) = 0;

signals:

  void unsavedChanges(bool);
};


class SettingsView
  : public DialogView
{

  Q_OBJECT

public:

  SettingsView(QWidget *parent = nullptr) : DialogView(parent) {}
  virtual ~SettingsView(){}

  /*!
   * \brief Active language
   */
  virtual QString activeLanguage() const = 0;

  /*!
   * \brief History size
   */
  virtual int historyMaxSize() const = 0;

  virtual QString imageViewerBGColor() const = 0;

  virtual bool useCuda() const = 0;

public slots:

  /*!
   * \brief Set active page
   * \param[in] page Active page
   */
  virtual void setPage(int page) = 0;

  /*!
   * \brief Set available languages
   * \param[in] languages Available languages
   */
  virtual void setLanguages(const QStringList &languages) = 0;

  /*!
   * \brief Set active language
   * \param[in] language Active language
   */
  virtual void setActiveLanguage(const QString &language) = 0;

  /*!
   * \brief Set history size
   * \param[in] size History size
   */
  virtual void setHistoryMaxSize(int size) = 0;

  virtual void setImageViewerBGcolor(const QString &color) = 0;
  virtual void setUseCuda(bool active) = 0;
  virtual void setCudaEnabled(bool enabled) = 0;
  virtual void setUnsavedChanges(bool unsaveChanges) = 0;

signals:

  void languageChange(QString);
  void historyMaxSizeChange(int);
  void imageViewerBGColorChange(QString);
  void useCudaChange(bool);
  void applyChanges();
};


class SettingsPresenter
  : public Presenter
{
  Q_OBJECT

public:

  SettingsPresenter() {}
  virtual ~SettingsPresenter(){}

public slots:

  virtual void openViewSettings() = 0;
  virtual void openToolSettings() = 0;

signals:

private slots:

  /*!
   * \brief Establece el idioma de la aplicación
   * \param[in] language
   */
  virtual void setLanguage(const QString &language) = 0;

  /*!
   * \brief save
   */
  virtual void save() = 0;

  /*!
   * \brief discart
   */
  virtual void discart() = 0;
};

} // namespace graphos

#endif // GRAPHOS_SETTINGS_INTERFACES_H
