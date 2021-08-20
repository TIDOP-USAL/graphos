#ifndef GRAPHOS_MATCH_VIEWER_VIEW_INTERFACE_H
#define GRAPHOS_MATCH_VIEWER_VIEW_INTERFACE_H

#include "graphos/interfaces/mvp.h"


namespace graphos
{

namespace ui
{

class MatchViewerView
  : public DialogView
{
  Q_OBJECT

public:

  explicit MatchViewerView(QWidget *parent = nullptr,
                            Qt::WindowFlags f = Qt::WindowFlags())
    : DialogView(parent, f) {}
  virtual ~MatchViewerView() = default;

  /*!
   * \brief Set the left image
   * \param[in] leftImage Left image
   */
  virtual void setLeftImage(const QString &leftImage) = 0;

  /*!
   * \brief Set the right image
   * \param[in] rightImage Right image
   */
  virtual void setRightImage(const QString &rightImage) = 0;

  /*!
   * \brief Set the list of images for image selector left
   * \param[in] leftImageList List of left images
   */
  virtual void setLeftImageList(const std::vector<QString> &leftImageList) = 0;

  /*!
   * \brief Set the list of images for image selector right
   * \param[in] rightImageList List of right images
   */
  virtual void setRightImageList(const std::vector<QString> &rightImageList) = 0;

  virtual void setMatches(const std::vector<std::tuple<size_t, size_t, QPointF, size_t, QPointF>> &matches) = 0;

  virtual void setBGColor(const QString &bgColor) = 0;
  virtual void setSelectedMarkerStyle(const QString &color, int width) = 0;
  virtual void setMarkerStyle(const QString &color, int width, int type = 0, int size = 20) = 0;
  virtual void setLineStyle(const QString &color, int width) = 0;

signals:

  void leftImageChange(QString);
  void rightImageChange(QString);
  void loadMatches(QString, QString);

};

} // namespace ui

} // namespace graphos


#endif // GRAPHOS_MATCH_VIEWER_VIEW_INTERFACE_H
