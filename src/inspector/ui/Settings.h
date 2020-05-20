#ifndef INSPECTOR_SETTINGS_INTERFACES_H
#define INSPECTOR_SETTINGS_INTERFACES_H

#include "inspector/interfaces/mvp.h"

namespace inspector
{

namespace ui
{

class SettingsModel
  : public IModel
{

  Q_OBJECT

public:

  SettingsModel(QObject *parent = nullptr) : IModel(parent) {}
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
  : public IDialogView
{

  Q_OBJECT

public:

  SettingsView(QWidget *parent = nullptr) : IDialogView(parent) {}
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
  : public IPresenter
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


} // namespace ui

} // namespace inspector

#endif // INSPECTOR_SETTINGS_INTERFACES_H
