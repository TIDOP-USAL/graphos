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

#ifndef GRAPHOS_PROGRESS_BAR_DIALOG_H
#define GRAPHOS_PROGRESS_BAR_DIALOG_H

#include "graphos/core/mvp.h"

class QLabel;
class QPushButton;
class QProgressBar;


namespace graphos
{

class ProgressBarDialog
  : public DialogView
{
    Q_OBJECT

public:

    explicit ProgressBarDialog(QWidget *parent = nullptr);
    ~ProgressBarDialog() override = default;

public slots:

    void setRange(int min, int max);
    void setValue(int value);
    void setInitialized();
    void setFinished();
    void setTitle(QString title);
    void setStatusText(const QString& text);
    void setCloseAuto(bool active = false);

protected slots:

    void onMinimized();
    void onPushButtonCancelClicked();

signals:

    void cancel();

// DialogView interface

private:

    void initUI() override;
    void initSignalAndSlots() override;

public slots:

    void clear() override;

private slots:

    void update() override;
    void retranslate() override;

// QDialog

    //void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:

    QLabel *mLabelStatus;
    QProgressBar *mProgressBar;
    QPushButton *mPushButtonCancel;
    QPushButton *mPushButtonMinimize;
    QPushButton *mPushButtonClose;
    bool mAutoClose;

};

} // namespace graphos


#endif // GRAPHOS_PROGRESS_BAR_DIALOG_H
