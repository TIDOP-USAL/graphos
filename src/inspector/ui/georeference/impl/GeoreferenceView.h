#ifndef INSPECTOR_GEOREFERENCE_VIEW_H
#define INSPECTOR_GEOREFERENCE_VIEW_H

#include "inspector/ui/georeference/GeoreferenceView.h"

class QDialogButtonBox;
class QTreeWidget;
class QTableView;
class QLabel;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QGridLayout;

namespace inspector
{

namespace ui
{

class GeoreferenceViewImp
  : public GeoreferenceView
{

  Q_OBJECT

public:

  GeoreferenceViewImp(QWidget *parent = nullptr);
  ~GeoreferenceViewImp() override;

protected slots:

  void openFile();

// GeoreferenceView interface

public:

  QString orientationFile() const override;

public slots:

  void setProjectPath(const QString &path) override;
  void setItemModel(QStandardItemModel *model) override;
  void setTableHeader(const QStringList &header) override;
  void setImageColumn(const QString &imageColumn) override;
  void setXColumn(const QString &xColumn) override;
  void setYColumn(const QString &yColumn) override;
  void setZColumn(const QString &zColumn) override;
  void setQxColumn(const QString &qxColumn) override;
  void setQyColumn(const QString &qyColumn) override;
  void setQzColumn(const QString &qzColumn) override;
  void setQwColumn(const QString &qwColumn) override;

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

  QHBoxLayout *mHorizontalLayoutImportCameras;
  QLineEdit *mLineEditImportCameras;
  QPushButton *mPushButtonImportCameras;
  QTableView *mTableViewImportCameras;
  QGridLayout *mGridLayoutSelectColmns;
  QLabel *mLabelImageColumn;
  QComboBox *mComboBoxImageColumn;
  QLabel *mLabelXColumn;
  QComboBox *mComboBoxXColumn;
  QLabel *mLabelYColumn;
  QComboBox *mComboBoxYColumn;
  QLabel *mLabelZColumn;
  QComboBox *mComboBoxZColumn;
  QLabel *mLabelQxColumn;
  QComboBox *mComboBoxQxColumn;
  QLabel *mLabelQyColumn;
  QComboBox *mComboBoxQyColumn;
  QLabel *mLabelQzColumn;
  QComboBox *mComboBoxQzColumn;
  QLabel *mLabelQwColumn;
  QComboBox *mComboBoxQwColumn;
  QLabel *mLabelCrsInput;
  QLineEdit *mLineEditCrsInput;
  QLabel *mLabelCrsOutput;
  QLineEdit *mLineEditCrsOutput;
  QHBoxLayout *mHorizontalLayoutCRS;
  //QPushButton *mPushButtonGeoreference;
  QTreeWidget *mTreeWidgetGeoreferencedCameras;
  QDialogButtonBox *mButtonBox;
  QString mProjectPath;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_VIEW_H
