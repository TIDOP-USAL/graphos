#include "MatchesModel.h"

#include <tidop/core/messages.h>

namespace inspector
{

namespace ui
{

 MatchesModelImp::MatchesModelImp(Project *project,
                                  QObject *parent)
   : MatchesModel(parent),
     mProject(project)
 {
   init();
 }

void MatchesModelImp::init()
{
}

void MatchesModelImp::clear()
{
  mProject->removeMatchesPair();
}

void MatchesModelImp::addMatchesPair(const QString &imageLeft,
                                     const QString &imageRight)
{
  mProject->addMatchesPair(imageLeft, imageRight);
}

const std::vector<QString> MatchesModelImp::matchesPairs(const QString &imageLeft) const
{
  return mProject->matchesPairs(imageLeft);
}

void MatchesModelImp::removeMatchesPair()
{
  mProject->removeMatchesPair();
}

void MatchesModelImp::removeMatchesPair(const QString &imageLeft)
{
  mProject->removeMatchesPair(imageLeft);
}

} // namespace ui

} // namespace inspector
