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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_PROGRESS_DIALOG_H
#define GRAPHOS_PROGRESS_DIALOG_H

#include <QDialog>
#include <QTextEdit>

#include "graphos/interfaces/mvp.h"
#include "graphos/process/Process.h"

namespace Ui
{
  class ProgressDialog;
}

namespace graphos
{

class ProgressDialog
  : public DialogView
{
  Q_OBJECT

public:

  explicit ProgressDialog(QWidget *parent = nullptr) : DialogView(parent){}
  virtual ~ProgressDialog() = default;

public slots:

  virtual void setRange(int min, int max) = 0;
  virtual void setValue(int value) = 0;
  virtual void setInitialized() = 0;
  virtual void setFinished() = 0;
  virtual void setTitle(QString text) = 0;
  virtual void setStatusText(QString text) = 0;
  virtual void setCloseAuto(bool active = false) = 0;

protected slots:

  virtual void onMinimized() = 0;

signals:

  void cancel();

};



class ProgressDialogImp
  : public ProgressDialog
{
  Q_OBJECT

public:

  explicit ProgressDialogImp(QWidget *parent = nullptr);
  ~ProgressDialogImp()  override;

// ProgressDialog interface

protected slots:

  void onPushButtonCancelClicked();

public slots:

  void setRange(int min, int max) override;
  void setValue(int value) override;
  void setInitialized() override;
  void setFinished() override;
  void setTitle(QString title) override;
  void setStatusText(QString text) override;
  void setCloseAuto(bool active = false) override;

protected slots:

  void onMinimized() override;

// DialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

private:

  Ui::ProgressDialog *ui;
  Process *mProcess;
  bool mAutoClose;
};

} // namespace graphos

#endif // GRAPHOS_PROGRESS_DIALOG_H
