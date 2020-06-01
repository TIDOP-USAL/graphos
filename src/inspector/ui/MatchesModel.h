#ifndef INSPECTOR_MATCHES_MODEL_H
#define INSPECTOR_MATCHES_MODEL_H

#include "inspector/interfaces/mvp.h"
#include "inspector/core/project.h"

namespace inspector
{

namespace ui
{

class MatchesModel
  : public IModel
{

  Q_OBJECT

public:

  MatchesModel(QObject *parent = nullptr) : IModel(parent) {}
  ~MatchesModel() override = default;

  virtual void addMatchesPair(const QString &imageLeft, const QString &imageRight) = 0;
  virtual const std::vector<QString> matchesPairs(const QString &imageLeft) const = 0;
  virtual void removeMatchesPair() = 0;
  virtual void removeMatchesPair(const QString &imageLeft) = 0;

};


class MatchesModelImp
  : public MatchesModel
{

  Q_OBJECT

public:

  MatchesModelImp(Project *project,
                  QObject *parent = nullptr);
  ~MatchesModelImp() override = default;

// MatchesModel interface

public:

  void addMatchesPair(const QString &imageLeft, const QString &imageRight) override;
  const std::vector<QString> matchesPairs(const QString &imageLeft) const override;
  void removeMatchesPair() override;
  void removeMatchesPair(const QString &imageLeft) override;

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

#endif // INSPECTOR_MATCHES_MODEL_H
