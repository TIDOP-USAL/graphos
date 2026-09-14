#include "Viewer3DContextMenu.h"

#include <QApplication>
#include <QAction>

namespace graphos
{

Viewer3DContextMenu::Viewer3DContextMenu(QWidget *parent)
  : GraphosContextMenu(parent),
    mActionGlobalZoom(new QAction(this)),
    mActionViewFront(new QAction(this)),
    mActionViewTop(new QAction(this)),
    mActionViewLeft(new QAction(this)),
    mActionViewRight(new QAction(this)),
    mActionViewBack(new QAction(this)),
    mActionViewBottom(new QAction(this)),
    mActionEDLFilter(new QAction(this)),
    mActionPerspective(new QAction(this)),
    mActionPointLighting(new QAction(this)),
    mActionPointSize(new QAction(this))
{
    init();
    initSignalAndSlots();
}

void Viewer3DContextMenu::setPerspectiveProjection(bool enabled)
{
    mActionPerspective->setChecked(enabled);
}

void Viewer3DContextMenu::setPointLighting(bool enabled)
{
    mActionPointLighting->setChecked(enabled);
}

void Viewer3DContextMenu::setPointLightingAvailable(bool available)
{
    mActionPointLighting->setEnabled(available);
    if (!available)
        mActionPointLighting->setChecked(false);
}

void Viewer3DContextMenu::init()
{
    mActionGlobalZoom->setIcon(QIcon::fromTheme("zoom-extend"));
    mActionViewFront->setIcon(QIcon::fromTheme("front-view"));
    mActionViewTop->setIcon(QIcon::fromTheme("top-view"));
    mActionViewLeft->setIcon(QIcon::fromTheme("left-view"));
    mActionViewRight->setIcon(QIcon::fromTheme("right-view"));
    mActionViewBack->setIcon(QIcon::fromTheme("back-view"));
    mActionViewBottom->setIcon(QIcon::fromTheme("bottom-view"));
    mActionEDLFilter->setCheckable(true);
    mActionPerspective->setCheckable(true);
    mActionPointLighting->setCheckable(true);
    addAction(mActionGlobalZoom);
    addSeparator();
    addAction(mActionViewFront);
    addAction(mActionViewTop);
    addAction(mActionViewLeft);
    addAction(mActionViewRight);
    addAction(mActionViewBack);
    addAction(mActionViewBottom);
    addSeparator();
    addAction(mActionEDLFilter);
    addSeparator();
    addAction(mActionPerspective);
    addAction(mActionPointLighting);
    addAction(mActionPointSize);
    retranslate();
}

void Viewer3DContextMenu::initSignalAndSlots()
{
    connect(mActionGlobalZoom, &QAction::triggered, this, &Viewer3DContextMenu::globalZoom);
    connect(mActionViewFront, &QAction::triggered, this, &Viewer3DContextMenu::viewFront);
    connect(mActionViewTop, &QAction::triggered, this, &Viewer3DContextMenu::viewTop);
    connect(mActionViewLeft, &QAction::triggered, this, &Viewer3DContextMenu::viewLeft);
    connect(mActionViewRight, &QAction::triggered, this, &Viewer3DContextMenu::viewRight);
    connect(mActionViewBack, &QAction::triggered, this, &Viewer3DContextMenu::viewBack);
    connect(mActionViewBottom, &QAction::triggered, this, &Viewer3DContextMenu::viewBottom);
    connect(mActionEDLFilter, &QAction::toggled, this, &Viewer3DContextMenu::filterEDL);
    connect(mActionPerspective, &QAction::toggled, this, &Viewer3DContextMenu::perspectiveProjection);
    connect(mActionPointLighting, &QAction::toggled, this, &Viewer3DContextMenu::pointLighting);
    connect(mActionPointSize, &QAction::triggered, this, &Viewer3DContextMenu::pointSize);
}

void Viewer3DContextMenu::retranslate()
{
    mActionGlobalZoom->setText(QApplication::translate("Viewer3DContextMenu", "Global Zoom", nullptr));
    mActionViewFront->setText(QApplication::translate("Viewer3DContextMenu", "View Front", nullptr));
    mActionViewTop->setText(QApplication::translate("Viewer3DContextMenu", "View Top", nullptr));
    mActionViewLeft->setText(QApplication::translate("Viewer3DContextMenu", "View Left", nullptr));
    mActionViewRight->setText(QApplication::translate("Viewer3DContextMenu", "View Right", nullptr));
    mActionViewBack->setText(QApplication::translate("Viewer3DContextMenu", "View Back", nullptr));
    mActionViewBottom->setText(QApplication::translate("Viewer3DContextMenu", "View Bottom", nullptr));
    mActionEDLFilter->setText(QApplication::translate("Viewer3DContextMenu", "EDL Filter", nullptr));
    mActionPerspective->setText(QApplication::translate("Viewer3DContextMenu", "Perspective Projection", nullptr));
    mActionPointLighting->setText(QApplication::translate("Viewer3DContextMenu", "Point Lighting", nullptr));
    mActionPointSize->setText(QApplication::translate("Viewer3DContextMenu", "Point Size...", nullptr));
}

}
