#include "ProjectControllerFake.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

namespace inspector
{

ProjectControllerFake::ProjectControllerFake()
{
  mProjectFileText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                     "<Inspector version=\"1.0\">"
                     "    <General>"
                     "        <Name>prj001</Name>"
                     "        <Path>C:/Users/User01/Documents/inspector/Projects/prj001</Path>"
                     "        <Description>Project example</Description>"
                     "    </General>"
                     "    <Images>"
                     "        <Image>"
                     "            <File>C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png</File>"
                     "            <LongitudeExif>0.5</LongitudeExif>"
                     "            <LatitudeExif>2.3</LatitudeExif>"
                     "            <AltitudeExif>10.2</AltitudeExif>"
                     "        </Image>"
                     "        <Image>"
                     "            <File>C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png</File>"
                     "            <LongitudeExif>0.51</LongitudeExif>"
                     "            <LatitudeExif>2.3</LatitudeExif>"
                     "            <AltitudeExif>10.1</AltitudeExif>"
                     "        </Image>"
                     "    </Images>"
                     "    <Database>C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db</Database>"
                     "</Inspector>";
}

bool ProjectControllerFake::read(const QString &file, Project &prj)
{
  QXmlStreamReader stream;
  stream.addData(mProjectFileText);

  if (stream.readNextStartElement()) {
      if (stream.name() == "Inspector") {
          while (stream.readNextStartElement()) {
              if (stream.name() == "General") {
                  readGeneral(stream, prj);
                } else if (stream.name() == "Database") {
                  readDatabase(stream, prj);
                } else if (stream.name() == "Images") {
                  readImages(stream, prj);
                }
            }
        } else {
          stream.raiseError(QObject::tr("Incorrect project file"));
          return true;
        }
    }

  return false;
}

bool ProjectControllerFake::write(const QString &file, const Project &prj) const
{
  return false;
}

bool ProjectControllerFake::checkOldVersion(const QString &file) const
{
  bool bUpdateVersion = false;

  return bUpdateVersion;
}

void ProjectControllerFake::oldVersionBak(const QString &file) const
{

}


} // end namespace inspector
