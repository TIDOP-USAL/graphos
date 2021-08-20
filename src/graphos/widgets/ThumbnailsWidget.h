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
  : public IWidgetView
{
  Q_OBJECT

public:

  explicit ThumbnailsWidget(QWidget *parent = nullptr);
  virtual ~ThumbnailsWidget() override {}

  void setActiveImage(const QString &imageName);
  void setActiveImages(const QStringList &imageNames);

signals:

  void openImage(QString);
  void selectImage(QString);
  void selectImages(QStringList);
  void deleteImages(QStringList);
  void imagesLoaded();

public slots:

  void addThumbnail(const QString &thumb);
  void addThumbnails(const QStringList &thumb);
  void deleteThumbnail(const QString &thumb);

private slots:

  void onThumbnailDoubleClicked(QListWidgetItem *item);
  void onSelectionChanged();

  void onThumbnailClicked();
  void onThumbnailSmallClicked();
  void onDetailsClicked();
  void onDeleteImageClicked();

  void showThumbnail(int id);
  void finished();

// IWidgetView interface

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
