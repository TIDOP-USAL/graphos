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

#ifndef GRAPHOS_LOADER_PRESENTER_H
#define GRAPHOS_LOADER_PRESENTER_H

#include "graphos/components/images/ImageLoaderPresenter.h"

namespace graphos
{

class Image;
class Camera;
class ImageLoaderView;
class ImageLoaderModel;
class HelpDialog;

class ImageLoaderPresenterImp
  : public ImageLoaderPresenter
{

  Q_OBJECT

public:

  ImageLoaderPresenterImp(ImageLoaderView *view,
                          ImageLoaderModel *model);
  ~ImageLoaderPresenterImp() override;

// ImageLoaderPresenter interface

public slots:

  void setImages(const QStringList &files) override;

protected slots:

  void addImage(int imageId, int cameraId) override;

// ProcessPresenter interface
  
private slots:

  void onError(int code, const QString &msg) override;
  void onFinished() override;
  bool createProcess() override;

public slots:
 
  void cancel() override;

// Presenter interface

public slots:

  void help() override;
  void open() override;
  void setHelp(HelpDialog *help) override;

private:

  void init() override;
  void initSignalAndSlots() override;

private:

  ImageLoaderView *mView;
  ImageLoaderModel *mModel;
  HelpDialog *mHelp;
  QStringList mImageFiles;
  std::vector<Image> mImages;
  std::vector<Camera> mCameras;
};

} // namespace graphos

#endif // GRAPHOS_LOADER_PRESENTER_H
