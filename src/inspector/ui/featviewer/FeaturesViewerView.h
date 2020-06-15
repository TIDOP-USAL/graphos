#ifndef INSPECTOR_FEATURES_VIEWER_VIEW_H
#define INSPECTOR_FEATURES_VIEWER_VIEW_H

#include "inspector/ui/featviewer/FeaturesViewer.h"

class QTreeWidget;
class QComboBox;
class QDialogButtonBox;
class QLabel;
class QMenu;

namespace inspector
{

class GraphicViewer;

namespace ui
{



class FeaturesViewerViewImp
  : public FeaturesViewerView
{
  Q_OBJECT

public:

  FeaturesViewerViewImp(QWidget *parent = nullptr,
                        Qt::WindowFlags f = Qt::WindowFlags());
  ~FeaturesViewerViewImp() override;

protected slots:

  void onGraphicsViewSelectionChanged();
  void onTreeWidgetItemSelectionChanged();

// FeaturesViewerView interface

  void setImageList(const std::vector<QString> &imageList) override;
  void setCurrentImage(const QString &leftImage) override;
  void setKeyPoints(const std::vector<QPointF> &keyPoints) override;
  void setBGColor(const QString &bgColor) override;
  void setSelectedMarkerStyle(const QString &color, int width) override;
  void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

// QWidget interface

protected:

  void closeEvent(QCloseEvent *event) override;

protected:

  QDialogButtonBox *mButtonBox;
  QLabel *mLabelImages;
  QComboBox *mComboBoxImages;
  GraphicViewer *mGraphicView;
  QTreeWidget *mTreeWidget;
  QMenu *mContextMenuLeft;
  QAction *mActionZoomIn;
  QAction *mActionZoomOut;
  QAction *mActionZoomExtend;
  QAction *mActionZoom11;
  QString mMarkerColor;
  QString mSelectedMarkerColor;
  int mMarkerType;
  int mMarkerSize;
  int mMarkerWidth;
  int mSelectedMarkerWidth;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_VIEW_H
