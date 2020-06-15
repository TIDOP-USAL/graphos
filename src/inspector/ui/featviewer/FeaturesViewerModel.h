#ifndef INSPECTOR_FEATURES_VIEWER_MODEL_H
#define INSPECTOR_FEATURES_VIEWER_MODEL_H

#include <memory>

#include "inspector/ui/featviewer/FeaturesViewer.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class FeaturesViewerModelImp
  : public FeaturesViewerModel
{

 Q_OBJECT

public:

  FeaturesViewerModelImp(Project *project,
                         QObject *parent = nullptr);
  ~FeaturesViewerModelImp() override;

// FeaturesViewerModel interface

public:

  std::vector<QString> images() const override;
  std::vector<QPointF> loadKeypoints(const QString &imageName) override;

// IModel interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;

};

} // namespace ui

} // namespace inspector

#endif // INSPECTOR_FEATURES_VIEWER_MODEL_H
