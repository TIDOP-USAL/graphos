#ifndef INSPECTOR_SETTINGS_H
#define INSPECTOR_SETTINGS_H

#include "inspector/inspector_global.h"

#include <QStringList>
#include <QSize>

class QSettings;

namespace inspector
{

/*!
 * \brief Settings interface
 */
class INSPECTOR_EXPORT Settings
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
  virtual QStringList history() const = 0;

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

  virtual bool useCuda() const = 0;
  virtual void setUseCuda(bool active) = 0;

  /*!
   * \brief Recupera la configuración por defecto
   */
  virtual void reset() = 0;

};


/*----------------------------------------------------------------*/


class INSPECTOR_EXPORT SettingsController
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

  virtual void writeHistory(const Settings &settings) = 0;
};



/*----------------------------------------------------------------*/


/*!
 * \brief The Settings class
 */
class INSPECTOR_EXPORT SettingsImp
  : public Settings
{

public:

  SettingsImp();
  ~SettingsImp() override;

// Settings interface

public:

  QString language() const override;
  void setLanguage(const QString &language) override;

  QStringList history() const override;
  void addToHistory(const QString &project) override;
  void clearHistory() override;
  int historyMaxSize() const override;
  void setHistoryMaxSize(int maxSize) override;

  QString imageViewerBGcolor() const override;
  void setImageViewerBGcolor(const QString &bgColor) override;

  bool useCuda() const override;
  void setUseCuda(bool active) override;

  void reset() override;

protected:

  QString mLanguage;
  int mHistoyMaxSize;
  QStringList mHistory;
  QString mImageViewerBGcolor;
  bool mUseCuda;

};


/*----------------------------------------------------------------*/


class INSPECTOR_EXPORT SettingsControllerImp
  : public SettingsController
{

public:

  explicit SettingsControllerImp();
  ~SettingsControllerImp() override;

// SettingsController interface

public:

  void read(Settings &settings) override;
  void write(const Settings &settings) override;
  void writeHistory(const Settings &settings) override;

protected:

  QSettings *mSettingsController;

};


} // namespace inspector

#endif // INSPECTOR_SETTINGS_H
