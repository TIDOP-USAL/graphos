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

/* TidopLib */
#include <tidop/core/msg/message.h>
#include <tidop/core/console.h>
#include <tidop/core/path.h>


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
 * \brief The IGraphicViewer class
 */
class Viewer3D
{

public:

    /*!
     * \brief TViewer3D
     */
    Viewer3D() {}
    virtual ~Viewer3D() {}

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
    virtual void addPrimitive(ccGenericPrimitive *primitive) = 0;
    virtual bool isEDL() const = 0;

protected:

};


class Application
    : public QApplication
{

    Q_OBJECT

public:

    static Application &instance();
    Application(int &argc, char **argv);
    ~Application();

    Application(const Application &) = delete;
    Application(Application &&) = delete;
    Application operator=(const Application &) = delete;
    Application operator=(Application &&) = delete;

    tl::Path documentsLocation() const;
    AppStatus *status();
    //tl::MessageManager *messageManager();

    Project *project();
    const Project *project() const;
    void setProject(Project *project);

    Settings *settings();

    QMainWindow *mainWindow() const;
    void setMainWindow(QMainWindow *mainWindow);

    Viewer3D *viewer3D() const;

    void addComponent(Component *component);
    bool parse(int argc, char **argv);
    bool runCommand();

    void freeMemory();

    QStringList history() const;
    void addToHistory(const QString &project);
    void clearHistory();

signals:

    void image_loaded(size_t);
    void update_history();

public slots:

    void setViewer3D(Viewer3D *viewer3D);

private:

    tl::CommandList *commandList();

private:

    static std::unique_ptr<Application> sApplication;
    static std::mutex sMutex;
    static std::once_flag sInitFlag;
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
