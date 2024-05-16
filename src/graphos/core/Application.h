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

#ifndef GRAPHOS_CORE_APPLICATION_H
#define GRAPHOS_CORE_APPLICATION_H

#include "graphos/graphos_global.h"

#include <memory>
#include <mutex>

/* Qt */
#include <QApplication>
#include <QImage>

/* TidopLib */
#include <tidop/core/msg/message.h>
#include <tidop/core/console.h>
#include <tidop/core/path.h>
#include <tidop/geometry/entities/point.h>
#include <tidop/graphic/color.h>

class QMainWindow;
class ccGenericPrimitive;

namespace tl
{
class CommandList;
class Command;
}


namespace graphos
{

class AppStatus;
class Component;
class Project;
class Settings;
class ColorTable;


/*!
 * \brief Viewer3D interface
 */
class Viewer3D
{

public:

    Viewer3D() = default;
    virtual ~Viewer3D() = default;

    virtual void clear() = 0;
    virtual void createGroup(const QString &group,
                             const QString &parent = QString()) = 0;
    virtual void deleteEntity(const QString &id) = 0;
    virtual void deleteSelectEntity() = 0;
    virtual void loadFromFile(const QString &file,
                              const QString &parent = QString()) = 0;
    virtual void loadFromFiles(const QStringList &files,
                               const QString &parent = QString()) = 0;

    /* Vistas por defecto */
    virtual void setFrontView() = 0;
    virtual void setBottomView() = 0;
    virtual void setTopView() = 0;
    virtual void setBackView() = 0;
    virtual void setLeftView() = 0;
    virtual void setRightView() = 0;
    virtual void setIsoView1() = 0;
    virtual void setIsoView2() = 0;
    virtual void setGlobalZoom() = 0;

    virtual void setVisible(const QString &id, bool visible) = 0;
    virtual void showClassification(bool show) = 0;
    virtual void setColorTable(std::shared_ptr<ColorTable> colorTable) = 0;
    virtual void enableEDL() = 0;
    virtual void disableEDL() = 0;
    virtual void addCamera(const QString &id,
                           double x,
                           double y,
                           double z,
                           const std::array<std::array<float, 3>, 3> &rot) = 0;
    virtual void drawLine(const std::string &name, 
                          const tl::Point3<double> &point1, 
                          const tl::Point3<double> &point2, 
                          const tl::Color &color, int width) = 0;
    virtual void addPrimitive(ccGenericPrimitive *primitive) = 0;
    virtual bool isEDL() const = 0;
    virtual QImage captureModel() = 0;

};


/*!
 * \brief Application class
 *
 * The `Application` class manage the application.
 * This class manages various aspects of the application, such as project management, settings, components, and UI.
 */
class Application
  : public QApplication
{

    Q_OBJECT

public:

    /*!
     * \brief Get the instance of the application
     *
     * Retrieves the singleton instance of the application.
     *
     * \return Reference to the application instance.
     */
    static Application &instance();

    /*!
     * \brief Constructor
     *
     * Constructs an `Application` object with the specified command line arguments.
     *
     * \param[in] argc Number of command line arguments.
     * \param[in] argv Array of command line arguments.
     */
    Application(int &argc, char **argv);

    /*!
     * \brief Destructor
     *
     * Destroys the `Application` object.
     */
    ~Application() override;

    TL_DISABLE_COPY(Application)
    TL_DISABLE_MOVE(Application)

    /*!
     * \brief Get the documents location
     *
     * Retrieves the location of the documents folder.
     *
     * \return Path to the documents folder.
     */
    static auto documentsLocation() -> tl::Path;

    /*!
     * \brief Get the application status
     *
     * Retrieves the status of the application.
     *
     * \return Pointer to the application status object.
     * \see AppStatus
     */
    auto status() const -> AppStatus*;

    /*!
     * \brief Get the project
     *
     * Retrieves the current project.
     *
     * \return Pointer to the project object.
     */
    auto project() -> Project*;

    /*!
     * \brief Get the project (const version)
     *
     * Retrieves the current project.
     *
     * \return Pointer to the project object.
     */
    auto project() const -> const Project*;

    /*!
     * \brief Set the current project
     *
     * Sets the current project.
     *
     * \param[in] project Pointer to the project object.
     */
    void setProject(Project *project);

    /*!
     * \brief Get the application settings
     *
     * Retrieves the application settings.
     *
     * \return Pointer to the settings object.
     */
    auto settings() const -> Settings*;


    /*!
     * \brief Get the main window
     *
     * Retrieves the main application window.
     *
     * \return Pointer to the main window object.
     */
    auto mainWindow() const -> QMainWindow*;

    /*!
     * \brief Set the main window
     *
     * Sets the main application window.
     *
     * \param[in] mainWindow Pointer to the main window object.
     */
    void setMainWindow(QMainWindow *mainWindow);

    /*!
     * \brief Get the 3D viewer
     *
     * Retrieves the 3D viewer component.
     *
     * \return Pointer to the 3D viewer object.
     */
    auto viewer3D() const -> Viewer3D*;

    /*!
     * \brief Add a component
     *
     * Adds a component to the application.
     *
     * \param[in] component Pointer to the component object.
     */
    void addComponent(Component *component);

    /*!
     * \brief Parse command line arguments
     *
     * Parses the command line arguments.
     *
     * \return True if parsing is successful, false otherwise.
     */
    auto parse(int argc, char** argv) -> bool;

    /*!
     * \brief Run a command
     *
     * Executes a command specified via command line arguments.
     *
     * \return True if the command is executed successfully, false otherwise.
     */
    auto runCommand() -> bool;

    /*!
     * \brief Free memory
     *
     * Frees memory occupied by loaded components.
     */
    void freeMemory() const;

    /*!
     * \brief Get the history of projects
     *
     * Retrieves the history of previously opened projects.
     *
     * \return List of project history.
     */
    auto history() const -> QStringList;

    /*!
     * \brief Add a project to history
     *
     * Adds a project to the history of opened projects.
     *
     * \param project Name of the project to add to history.
     */
    void addToHistory(const QString &project);

    /*!
     * \brief Clear project history
     *
     * Clears the history of opened projects.
     */
    void clearHistory();

signals:

    /*!
     * \brief Signal emitted when an image is loaded
     *
     * This signal is emitted when an image is loaded into the application.
     *
     * \param imageId ID of the loaded image.
     */
    void image_loaded(size_t);

    /*!
     * \brief Signal emitted when the project history is updated
     *
     * This signal is emitted when the project history is updated.
     */
    void update_history();

public slots:

    /*!
     * \brief Set the 3D viewer
     *
     * Sets the 3D viewer component.
     *
     * \param viewer3D Pointer to the 3D viewer object.
     */
    void setViewer3D(Viewer3D *viewer3D);

private:

    tl::CommandList *commandList();

private:

    AppStatus *mAppStatus;
    Project *mProject;
    Settings *mSettings;
    QMainWindow *mMainWindow;
    std::list<Component *> mComponents;
    tl::CommandList *mCommandList;
    mutable QStringList mHistory;
    Viewer3D *mViewer3D;
};

} // namespace graphos


#endif // GRAPHOS_CORE_APPLICATION_H
