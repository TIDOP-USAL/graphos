#ifndef INSPECTOR_MATCH_VIEWER_VIEW_H
#define INSPECTOR_MATCH_VIEWER_VIEW_H

#include "inspector/ui/matchviewer/MatchViewerView.h"

class QDialogButtonBox;
class QTreeWidget;
class QComboBox;
class QTreeWidgetItem;
class QLabel;

namespace inspector
{

class GraphicViewer;

namespace ui
{

class MatchViewerViewImp
  : public MatchViewerView
{

  Q_OBJECT

public:

  MatchViewerViewImp(QWidget *parent = nullptr,
                     Qt::WindowFlags f = Qt::WindowFlags());
  ~MatchViewerViewImp() override;

protected slots :

  void onComboBoxLeftImageIndexChanged(int idx);
  void onComboBoxRightImageIndexChanged(int idx);
  void onTreeWidgetMatchesItemClicked(QTreeWidgetItem *item, int col);
  void onTreeWidgetMatchesItemSelectionChanged();
  void onGraphicsViewLeftSelectionChanged();
  void onGraphicsViewRightSelectionChanged();
//  void onPushButtonDeleteMatchClicked();

// IMatchViewerView interface

public:

  void setLeftImage(const QString &leftImage) override;
  void setRightImage(const QString &rightImage) override;
  void setLeftImageList(const std::vector<QString> &leftImageList) override;
  void setRightImageList(const std::vector<QString> &rightImageList) override;
  void setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> &matches) override;
  void setBGColor(const QString &bgColor) override;
  void setSelectedMarkerStyle(const QString &color, int width) override;
  void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) override;
  void setLineStyle(const QString &color, int width) override;

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
  QTreeWidget *mTreeWidgetMatches;
  QLabel *mLabelLeftImage;
  QLabel *mRightImage;
  QComboBox  *mComboBoxLeftImage;
  QComboBox  *mComboBoxRightImage;
  GraphicViewer *mGraphicsViewRight;
  GraphicViewer *mGraphicsViewLeft;
  QLabel *mLabelMatches;
  QPushButton *mPushButtonDeleteMatch;
  QAction *mActionLeftZoomIn;
  QAction *mActionLeftZoomOut;
  QAction *mActionLeftZoomExtend;
  QAction *mActionLeftZoom11;
  QAction *mActionRightZoomIn;
  QAction *mActionRightZoomOut;
  QAction *mActionRightZoomExtend;
  QAction *mActionRightZoom11;
  bool bUnsavedChanges;
  QString mMarkerColor;
  int mMarkerSize;
  int mMarkerWidth;
  int mMarkerType;
  QString mLineColor;
  int mLineWidth;
  QString mSelectedMarkerColor;
  int mSelectedMarkerWidth;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_MATCH_VIEWER_VIEW_H
