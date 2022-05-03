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


#include "graphos/widgets/GraphosWidget.h"

#include <QImage>

class QListWidget;
class QListWidgetItem;
class QGridLayout;
template <typename T> class QFutureWatcher;

namespace graphos
{

class GRAPHOS_EXPORT ThumbnailsWidget
  : public GraphosWidgetView
{
  Q_OBJECT

public:

  explicit ThumbnailsWidget(QWidget *parent = nullptr);
  virtual ~ThumbnailsWidget() override {}

  void setActiveImage(size_t imageId);
  void setActiveImages(const std::vector<size_t> &imageIds);

signals:

  void openImage(size_t);
  void selectImage(size_t);
  void selectImages(std::vector<size_t>);
  void delete_images(std::vector<size_t>);
  void imagesLoaded();

public slots:

  void addImage(const QString &thumb, size_t imageId);
  void addThumbnails(const QStringList &thumb);
  void deleteImages(const std::vector<size_t> &imageIds);

private slots:

  void onThumbnailDoubleClicked(QListWidgetItem *item);
  void onSelectionChanged();

  void onThumbnailClicked();
  void onThumbnailSmallClicked();
  void onDetailsClicked();
  void onDeleteImageClicked();

  void showThumbnail(int id);
  void finished();

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

protected:

  QListWidget *mListWidget;
  QGridLayout *mGridLayout;
  QSize mIconSize;
  QAction *mThumbnailAction;
  QAction *mThumbnailSmallAction;
  QAction *mDetailsAction;
  QAction *mDeleteImageAction;
  QFutureWatcher<QImage> *mFutureWatcherThumbnail;
  int mThumbnaislSize;
  bool bLoadingImages;
};

} // namespace graphos

#endif // GRAPHOS_THUMBNAILS_WIDGET_H
