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

#ifndef GRAPHOS_THUMBNAILS_WIDGET_H
#define GRAPHOS_THUMBNAILS_WIDGET_H

#include <unordered_map>

#include <QImage>

#include <tidop/core/task.h>

#include "graphos/widgets/GraphosWidget.h"
#include "graphos/core/image.h"

class QListWidget;
class QListWidgetItem;
class QGridLayout;

namespace graphos
{


class LoadThumbnailTask
  : public QObject,
    public tl::TaskBase
{

  Q_OBJECT

public:

  LoadThumbnailTask(QListWidgetItem *item, 
                    QListWidget *listWidget);
  ~LoadThumbnailTask() override;

signals:

private:

// tl::TaskBase interface

protected:

  void execute(tl::Progress *progressBar) override;

protected:

  QListWidgetItem *mItem;
  QListWidget *mListWidget;
};


class ThumbnailsWidget
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  explicit ThumbnailsWidget(QWidget *parent = nullptr);
  ~ThumbnailsWidget() override;

  void setActiveImage(size_t imageId);
  void setActiveImages(const std::vector<size_t> &imageIds);

signals:

  void open_image(size_t);
  void select_image(size_t);
  void select_images(std::vector<size_t>);
  void delete_images(std::vector<size_t>);

public slots:

  void addThumbnail(const Image &image, const QSize &imageSize);
  void deleteImages(const std::vector<size_t> &imageIds);

private slots:

  void onThumbnailDoubleClicked(QListWidgetItem *item);
  void onSelectionChanged();
  void loadVisibleImages();
  void onThumbnailClicked();
  void onThumbnailSmallClicked();
  void onDetailsClicked();
  void onDeleteImageClicked();

// GraphosWidgetView interface

protected slots:

  void update() override;
  void retranslate() override;

public slots:

  void clear() override;

private:

  virtual void initUI() override;
  void initSignalAndSlots() override;

// QWidget interface

protected:

  void changeEvent(QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

protected:

  QListWidget *mListWidget;
  QGridLayout *mGridLayout;
  QSize mIconSize;
  QAction *mThumbnailAction;
  QAction *mThumbnailSmallAction;
  QAction *mDetailsAction;
  QAction *mDeleteImageAction;
  int mThumbnaislSize;
  bool bLoadingImages;
  tl::TaskQueue *mThumbLoad;
  bool mLoadImages;
};

} // namespace graphos

#endif // GRAPHOS_THUMBNAILS_WIDGET_H
