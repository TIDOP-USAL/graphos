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
#include <QDialog>
#include <QEvent>
TL_DEFAULT_WARNINGS

namespace graphos
{

class DialogView
  : public QDialog,
    public View
{
    Q_OBJECT

public:

    DialogView(QWidget *parent = nullptr,
               Qt::WindowFlags f = Qt::WindowFlags())
      : QDialog(parent, f)
    {
        Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
        Qt::WindowFlags flags = windowFlags();
        flags = flags & (~helpFlag);
        QWidget::setWindowFlags(flags);
    }

    ~DialogView() override = default;

private:

    /*!
     * \brief Dialog initialization UI
     */
    virtual void initUI() = 0;

    /*!
     * \brief Signal and slots initialization
     */
    virtual void initSignalAndSlots() = 0;

signals:

    void help();

//public slots:
//
//    /*!
//     * \brief Clear dialog
//     */
//    virtual void clear() = 0;

private slots:

    virtual void update() = 0;
    virtual void retranslate() = 0;

// View interface

public:

    auto widget() -> QWidget *override
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
    }
};

} // namespace graphos
