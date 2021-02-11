#ifndef INSPECTOR_GEOREFERENCE_VIEW_H
#define INSPECTOR_GEOREFERENCE_VIEW_H

#include "inspector/ui/georeference/GeoreferenceView.h"

#include <tidop/geometry/entities/point.h>

#include <QItemSelection>

class QDialogButtonBox;
//class QTreeWidget;
class QLabel;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QMenu;
class QTableView;

namespace inspector
{

class GraphicViewer;


namespace ui
{

class GeoreferenceViewImp
  : public GeoreferenceView
{

  Q_OBJECT

public:

  GeoreferenceViewImp(QWidget *parent = nullptr,
                      Qt::WindowFlags f = Qt::WindowFlags());
  ~GeoreferenceViewImp() override;

protected slots:

  void openFile();
  void onGraphicsViewSelectionChanged();
  void onGraphicsViewRemoveSelectItems();

  void selectionChanged(const QItemSelection &selected, 
                        const QItemSelection &deselected);
  //void onTreeWidgetItemSelectionChanged();
  void clickedPoint(const QPointF &point);

// GeoreferenceView interface

public:

  //QString orientationFile() const override;

public slots:

  void setProjectPath(const QString &path) override;
  void setImageList(const std::vector<QString> &imageList) override;
  void setCurrentImage(const QString &leftImage) override;
  void setItemModelGroundControlPoints(QAbstractItemModel *model) override;
  void setItemModelImagePoints(QAbstractItemModel *model) override;
  //void setTableHeader(const QStringList &header) override;
  void setEnableImagePointsAddOrEdit(bool active) override;
  void setPoints(const std::list<std::pair<QString, QPointF>> &points) override;
  void setCrs(const QString &crs) override;

private slots: 

  void removeGroundControlPoints() override;

// IDialogView interface

private:

  void initUI() override;
  void initSignalAndSlots() override;

public slots:

  void clear() override;

private slots:

  void update() override;
  void retranslate() override;

protected:

  QLabel *mLabelCRS;
  QLineEdit *mLineEditCRS;
  //QTreeWidget *mTreeWidgetGroundControlPoints;
  QTableView *mTableViewGroundControlPoints;
  QPushButton *mPushButtonAddPoint;
  QPushButton *mPushButtonDeletePoint;
  QPushButton *mPushButtonGeoreference;
  QSpacerItem *verticalSpacer;
  
  //QTreeWidget *mTreeWidgetImagePoints;
  QLabel *mLabelFilterByControlPoint;
  QComboBox *mComboBoxFilterByControlPoint;
  QLabel *mLabelFilterByImage;
  QComboBox *mComboBoxFilterByImage;
  QLabel *mLabelImagePoints;
  QTableView *mTableViewImagePoints;
  GraphicViewer *mGraphicViewerWidget;
  QLabel *mLabelImage;
  QComboBox *mComboBoxImages;
  QDialogButtonBox *mButtonBox;
  QString mProjectPath;
  QMenu *mContextMenuLeft;
  QAction *mActionZoomIn;
  QAction *mActionZoomOut;
  QAction *mActionZoomExtend;
  QAction *mActionZoom11;

  bool bSelectedItem;
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_VIEW_H
