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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/

#ifndef GRAPHOS_ORIENTATION_FORMAT_NVM_WIDGETS_H
#define GRAPHOS_ORIENTATION_FORMAT_NVM_WIDGETS_H

#include "graphos/widgets/GraphosWidget.h"


class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace graphos
{

class GRAPHOS_EXPORT NvmFormatWidget
  : public GraphosWidgetView
{

  Q_OBJECT

public:

  NvmFormatWidget(QWidget *parent = nullptr) : GraphosWidgetView(parent){}
  ~NvmFormatWidget() override = default;

  virtual QString file() const = 0;

signals:

  void fileChanged(QString);

public slots:

  virtual void setFile(const QString &file) = 0;

};

class GRAPHOS_EXPORT NvmFormatWidgetImp
  : public NvmFormatWidget
{

  Q_OBJECT

public:

  NvmFormatWidgetImp(QWidget *parent = nullptr);
  ~NvmFormatWidgetImp() override = default;

protected slots:

  void onPushButtonSelectPath();

// NvmFormatWidget interface

  QString file() const override;

signals:

  void fileChanged(const QString &);

public slots:

  void setFile(const QString &file) override;

// GraphosWidgetView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QGroupBox *mGroupBox;
  QLabel *mLabelFile;
  QLineEdit *mLineEditFile;
  QPushButton *mPushButtonSelectPath;

};

} // namespace graphos

#endif // GRAPHOS_ORIENTATION_FORMAT_NVM_WIDGETS_H
