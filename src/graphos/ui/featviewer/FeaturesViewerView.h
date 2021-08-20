#ifndef GRAPHOS_FEATURES_VIEWER_VIEW_INTERFACE_H
#define GRAPHOS_FEATURES_VIEWER_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"

namespace graphos
{

namespace ui
{


class FeaturesViewerView
  : public DialogView
{
  Q_OBJECT

public:

  explicit FeaturesViewerView(QWidget *parent = nullptr,
                               Qt::WindowFlags f = Qt::WindowFlags())
       : DialogView(parent, f) {}
  ~FeaturesViewerView() = default;

  /*!
   * \brief Set the list of images
   * \param[in] imageList List of images
   */
  virtual void setImageList(const std::vector<QString> &imageList) = 0;

  /*!
   * \brief setCurrentImage
   * \param leftImage
   */
  virtual void setCurrentImage(const QString &leftImage) = 0;

  /*!
   * \brief setPoints
   * \param[in] keyPoints Keypoints
   */
  virtual void setKeyPoints(const std::vector<QPointF> &keyPoints) = 0;

  virtual void setBGColor(const QString &bgColor) = 0;
  virtual void setSelectedMarkerStyle(const QString &color, int width) = 0;
  virtual void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) = 0;

signals:

  void imageChange(QString);

};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_FEATURES_VIEWER_VIEW_INTERFACE_H
