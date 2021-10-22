#include "ProjectFake.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

namespace graphos
{

ProjectFake::ProjectFake()
{
  mProjectFileText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                     "<Graphos version=\"1.0\">"
                     "    <General>"
                     "        <Name>prj001</Name>"
                     "        <Path>C:/Users/User01/Documents/graphos/Projects/prj001</Path>"
                     "        <Description>Project 1</Description>"
                     "    </General>"
                     "</Graphos>";
}

bool ProjectFake::load(const QString &file)
{
  QXmlStreamReader stream;
  stream.addData(mProjectFileText);

  return this->read(stream);
}

} // end namespace graphos
