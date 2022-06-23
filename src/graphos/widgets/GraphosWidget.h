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

#ifndef GRAPHOS_WIDGET_H
#define GRAPHOS_WIDGET_H

#include "graphos/graphos_global.h"

#include <memory>

#include <QWidget>
#include <QEvent>
#include <QMenu>

namespace graphos
{

class GraphosWidgetView
  : public QWidget
{

  Q_OBJECT

public:

  GraphosWidgetView(QWidget *parent)
    : QWidget(parent)
  {
  }

  virtual ~GraphosWidgetView() override {}

private:

  /*!
   * \brief Widget Initialization
   */
  virtual void initUI() = 0;
  virtual void initSignalAndSlots() = 0;

public slots:

  /*!
   * \brief Clear the widget
   */
  virtual void clear() = 0;

private slots:

  virtual void update() = 0;
  virtual void retranslate() = 0;

// QWidget interface

protected:

  void changeEvent(QEvent *event) override
  {
    if (event->type() == QEvent::LanguageChange){
      this->retranslate();
    }
  }

};


class GraphosContextMenu
  : public QMenu
{

  Q_OBJECT

public:

  GraphosContextMenu(QWidget *parent)
    : QMenu(parent)
  {
  }

  virtual ~GraphosContextMenu() override {}

private slots:

  virtual void retranslate() = 0;

// QWidget interface

protected:

  void changeEvent(QEvent *event) override
  {
    if (event->type() == QEvent::LanguageChange) {
      this->retranslate();
    }
  }

};

} // namespace graphos

#endif // GRAPHOS_WIDGET_H
