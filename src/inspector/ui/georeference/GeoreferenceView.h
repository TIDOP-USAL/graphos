#ifndef INSPECTOR_GEOREFERENCE_VIEW_INTERFACE_H
#define INSPECTOR_GEOREFERENCE_VIEW_INTERFACE_H

#include "inspector/interfaces/mvp.h"

class QAbstractItemModel;

namespace inspector
{

namespace ui
{

class GeoreferenceView
  : public DialogView
{

  Q_OBJECT

public:

  GeoreferenceView(QWidget *parent,
                   Qt::WindowFlags f = Qt::WindowFlags()) 
    : DialogView(parent, f) {}
  ~GeoreferenceView() override = default;

  //virtual QString orientationFile() const = 0;

public slots:

  virtual void setProjectPath(const QString &path) = 0;
  virtual void setImageList(const std::vector<QString> &imageList)  = 0;
  virtual void setCurrentImage(const QString &image)  = 0;
  virtual void setItemModelGroundControlPoints(QAbstractItemModel *model) = 0;
  virtual void setItemModelImagePoints(QAbstractItemModel *model) = 0;
  //virtual void setTableHeader(const QStringList &header) = 0;
  virtual void setEnableImagePointsAddOrEdit(bool active) = 0;
  virtual void setPoints(const std::list<std::pair<QString, QPointF>> &points) = 0;
  virtual void setCrs(const QString &crs) = 0;

private slots: 

  virtual void removeGroundControlPoints() = 0;

signals:

  void imageChange(QString);
  void crsChange(QString);
  //void loadCSV(const QString &, const QString &);
  void addGroundControlPoint();
  void removeGroundControlPoint(int);
  void addImagePoint(const QString &, const QString &, const QPointF &);
  void removeImagePoint(const QString &, const QString &);
  void georeference();
};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_GEOREFERENCE_VIEW_INTERFACE_H
