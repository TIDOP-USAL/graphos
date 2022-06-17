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


#ifndef GRAPHOS_LOADFROMVIDEO_VIEW_H
#define GRAPHOS_LOADFROMVIDEO_VIEW_H

#include "graphos/components/loadfromvideo/LoadFromVideoView.h"

class QDialogButtonBox;
class QLabel;
class QComboBox;
class QLabel;
class QSpinBox;


namespace graphos
{

class LoadFromVideoViewImp
  : public LoadFromVideoView
{

  Q_OBJECT

public:

  LoadFromVideoViewImp(QWidget *parent = nullptr);
  ~LoadFromVideoViewImp() override;

// LoadFromVideoView

public:

  QString Video() const override;
  int SkipFrames() const override;

public slots:
  
  void setVideo(const QString &Video) override;
  void setSkipFrames(int SkipFrames) override;

// DialogView

private:

  void initUI();
  void initSignalAndSlots();

public slots:

  void clear();

private slots:

  void update();
  void retranslate();

protected:

  QLabel *mLabelVideo;
  QComboBox *mComboBoxVideo;
  QLabel *mLabelSkipFrames;
  QSpinBox *mSpinBoxSkipFrames;
  QDialogButtonBox *mButtonBox;
  
};

} // namespace graphos

#endif // GRAPHOS_LOADFROMVIDEO_VIEW_H
