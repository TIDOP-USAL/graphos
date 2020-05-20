#include "ProjectFake.h"

#include <QFile>
#include <QFileInfo>
#include <QXmlStreamWriter>

namespace inspector
{

ProjectFake::ProjectFake()
{
  mProjectFileText = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                     "<Inspector version=\"1.0\">"
                     "    <General>"
                     "        <Name>prj001</Name>"
                     "        <Path>C:/Users/User01/Documents/inspector/Projects/prj001</Path>"
                     "        <Description>Project example</Description>"
                     "    </General>"
                     "    <Cameras>"
                     "        <Camera id=\"1\">"
                     "            <Make>DJI</Make>"
                     "            <Model>FC6310</Model>"
                     "            <Type>RADIAL</Type>"
                     "            <Focal>3752.23</Focal>"
                     "            <Width>5472</Width>"
                     "            <Height>3648</Height>"
                     "            <SensorSize>12.8333</SensorSize>"
//                     "            <Calibration type=\"SIMPLE_RADIAL\">"
//                     "                <f>0</f>"
//                     "                <cx>0</cx>"
//                     "                <cy>0</cy>"
//                     "                <k1>0</k1>"
//                     "            </Calibration>"
                     "        </Camera>"
                     "    </Cameras>"
                     "    <Cameras>"
                     "        <Camera id=\"2\">"
                     "            <Make>Unknown camera</Make>"
                     "            <Model>0</Model>"
                     "            <Type>SIMPLE_RADIAL</Type>"
                     "            <Focal>4753.2</Focal>"
                     "            <Width>3961</Width>"
                     "            <Height>2968</Height>"
                     "            <SensorSize>1</SensorSize>"
                     "        </Camera>"
                     "    </Cameras>"
                     "    <Images>"
                     "        <Image>"
                     "            <File>C:/Users/User01/Documents/inspector/Projects/prj001/images/img001.png</File>"
                     "            <CameraId>1</CameraId>"
                     "            <LongitudeExif>0.5</LongitudeExif>"
                     "            <LatitudeExif>2.3</LatitudeExif>"
                     "            <AltitudeExif>10.2</AltitudeExif>"
                     "        </Image>"
                     "        <Image>"
                     "            <File>C:/Users/User01/Documents/inspector/Projects/prj001/images/img002.png</File>"
                     "            <CameraId>1</CameraId>"
                     "            <LongitudeExif>0.51</LongitudeExif>"
                     "            <LatitudeExif>2.3</LatitudeExif>"
                     "            <AltitudeExif>10.1</AltitudeExif>"
                     "        </Image>"
                     "    </Images>"
                     "    <Database>C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db</Database>"
                     "    <Features>"
                     "        <FeatureExtractor>"
                     "            <SIFT>"
                     "                <FeaturesNumber>5000</FeaturesNumber>"
                     "                <OctaveLayers>3</OctaveLayers>"
                     "                <ContrastThreshold>0.04</ContrastThreshold>"
                     "                <EdgeThreshold>10</EdgeThreshold>"
                     "                <Sigma>1.6</Sigma>"
                     "            </SIFT>"
                     "        </FeatureExtractor>"
                     "        <Files>"
                     "            <FeatFile id=\"img001\">img001@C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db</FeatFile>"
                     "            <FeatFile id=\"img002\">img002@C:/Users/User01/Documents/inspector/Projects/prj001/prj001.db</FeatFile>"
                     "        </Files>"
                     "    </Features>"
                     "</Inspector>";
}

bool ProjectFake::load(const QString &file)
{
  QXmlStreamReader stream;
  stream.addData(mProjectFileText);

  if (stream.readNextStartElement()) {
      if (stream.name() == "Inspector") {
          while (stream.readNextStartElement()) {
              if (stream.name() == "General") {
                  readGeneral(stream);
                } else if (stream.name() == "Database") {
                  readDatabase(stream);
                } else if (stream.name() == "Cameras") {
                  readCameras(stream);
                } else if (stream.name() == "Images") {
                  readImages(stream);
                }
            }
        } else {
          stream.raiseError(QObject::tr("Incorrect project file"));
          return true;
        }
    }

  return false;
}

} // end namespace inspector
