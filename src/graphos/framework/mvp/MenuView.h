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

#pragma once

#include "graphos/graphos_global.h"

#include "graphos/framework/mvp/View.h"

TL_DISABLE_WARNINGS
#include <QMenu>
#include <QEvent>
TL_DEFAULT_WARNINGS

namespace graphos
{

class MenuView
  : public QMenu,
    public View
{
    Q_OBJECT

public:

    explicit MenuView(QWidget *parent = nullptr)
      : QMenu(parent)
    {
    }
    
    explicit MenuView(const QString &title, QWidget *parent = nullptr)
      : QMenu(title, parent)
    {
    }

    ~MenuView() override = default;

private:

    virtual void init() = 0;

private slots:

    virtual void retranslate() = 0;

// View interface

public:

    auto widget() -> QWidget * override
    {
        return this;
    }

// QWidget interface

protected:

    void changeEvent(QEvent *event) override
    {
        if (event->type() == QEvent::LanguageChange) {
            retranslate();
        }
        QMenu::changeEvent(event);
    }
};

} // namespace graphos
