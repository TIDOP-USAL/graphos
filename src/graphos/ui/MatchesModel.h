#ifndef GRAPHOS_MATCHES_MODEL_H
#define GRAPHOS_MATCHES_MODEL_H

#include "graphos/interfaces/mvp.h"
#include "graphos/core/project.h"

namespace graphos
{

namespace ui
{

class MatchesModel
  : public Model
{

  Q_OBJECT

public:

  MatchesModel(QObject *parent = nullptr) : Model(parent) {}
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

// Model interface

private:

  void init() override;

public slots:

  void clear() override;

protected:

  Project *mProject;
};

} // namespace ui

} // namespace graphos

#endif // GRAPHOS_MATCHES_MODEL_H
