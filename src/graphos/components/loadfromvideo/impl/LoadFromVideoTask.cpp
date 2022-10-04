/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.es>          *
 *                                                                      *
 * This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can *
 * redistribute it and/or modify it under the terms of the GNU General  *
 * Public License as published by the Free Software Foundation, either  *
 * version 3 of the License, or (at your option) any later version.     *
 *                                                                      *
 * GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the     *
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even  *
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  *
 * PURPOSE.  See the GNU General Public License for more details.       *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "LoadFromVideoTask.h"

#include "graphos/core/utils.h"
#include "graphos/core/camera/Camera.h"
#include "graphos/core/camera/Database.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>
#include <tidop/core/progress.h>
#include <tidop/img/imgwriter.h>
#include <tidop/img/metadata.h>
#include <tidop/math/angles.h>
#include <tidop/geospatial/crstransf.h>
#include <tidop/geospatial/util.h>

#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

constexpr auto DefaultImportVideoFramesSkipFrames = 20;

class ImportVideoFramesAlgorithm
{

public:

  ImportVideoFramesAlgorithm();
  ~ImportVideoFramesAlgorithm();

  bool open();
  cv::Mat read();

  tl::Path video() const;
  void setVideo(const tl::Path &video);

  double framesPerSecond() const;
  int width() const;
  int height() const;
  int framePosition() const;
  int skipFrames() const;
  void setSkipFrames(int skipFrames);
  void startAt(int frame);
  void finishAt(int frame);

  void clear();

private:

  tl::Path mVideo;

  cv::VideoCapture mVideoCapture;
  double mFramesPerSecond;
  int mCodec;
  int mWidth{0};
  int mHeight{0};
  int mSkipFrames;
  int mStartAt{0};
  int mFinishAt{-1};
};





/* ImportVideoFramesAlgorithm */

ImportVideoFramesAlgorithm::ImportVideoFramesAlgorithm()
  : mSkipFrames(DefaultImportVideoFramesSkipFrames)
{
}

ImportVideoFramesAlgorithm::~ImportVideoFramesAlgorithm()
{
}

int ImportVideoFramesAlgorithm::skipFrames() const 
{
  return mSkipFrames;
}

void ImportVideoFramesAlgorithm::setSkipFrames(int skipFrames) 
{
  mSkipFrames = skipFrames;
}

void ImportVideoFramesAlgorithm::startAt(int frame)
{
  mStartAt = frame;
}

void ImportVideoFramesAlgorithm::finishAt(int frame)
{
  mFinishAt = frame;
}

void ImportVideoFramesAlgorithm::clear()
{
  mSkipFrames = DefaultImportVideoFramesSkipFrames;
}

bool ImportVideoFramesAlgorithm::open()
{

  if (!mVideoCapture.open(mVideo.toString())) {
    return false;
  } else {

    mWidth = static_cast<int>(mVideoCapture.get(cv::CAP_PROP_FRAME_WIDTH));
    mHeight = static_cast<int>(mVideoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));
    msgInfo("Video size: %ix%i", mWidth, mHeight);

    mFramesPerSecond = mVideoCapture.get(cv::CAP_PROP_FPS);
    msgInfo("Framerate: %f", mFramesPerSecond);

    mCodec = static_cast<int>(mVideoCapture.get(cv::CAP_PROP_FOURCC));
    char c[] = {(char)(mCodec & 0XFF) , (char)((mCodec & 0XFF00) >> 8),(char)((mCodec & 0XFF0000) >> 16),(char)((mCodec & 0XFF000000) >> 24), 0};
    msgInfo("Video codec: %f", c);
    
    mVideoCapture.set(cv::CAP_PROP_POS_FRAMES, mStartAt - 1);

    int frames = static_cast<int>(mVideoCapture.get(cv::CAP_PROP_FRAME_COUNT));
    if (mFinishAt == -1 || mFinishAt > frames) {
      mFinishAt = frames;
    }

    return true;
  }
}

cv::Mat ImportVideoFramesAlgorithm::read()
{

  cv::Mat frame;

  try {

    double posframe;
    
    int current_frame = mVideoCapture.get(cv::CAP_PROP_POS_FRAMES);

    if (current_frame < mFinishAt && mVideoCapture.read(frame)) {
      
      posframe = mVideoCapture.get(cv::CAP_PROP_POS_FRAMES);
      int next_frame = posframe - 1 + skipFrames();
      mVideoCapture.set(cv::CAP_PROP_POS_FRAMES, next_frame);
    }

  } catch (std::exception &e) {
    msgError("Error de lectura: %s", e.what());
  }

  return frame;
}

tl::Path ImportVideoFramesAlgorithm::video() const
{
  return mVideo;
}

void ImportVideoFramesAlgorithm::setVideo(const tl::Path &video)
{
  mVideo = video;
}

double ImportVideoFramesAlgorithm::framesPerSecond() const
{
  return mFramesPerSecond;
}

int ImportVideoFramesAlgorithm::width() const
{
  return mWidth;
}

int ImportVideoFramesAlgorithm::height() const
{
  return mHeight;
}

int ImportVideoFramesAlgorithm::framePosition() const
{
  return mVideoCapture.get(cv::CAP_PROP_POS_FRAMES);
}



namespace graphos
{

LoadFromVideoTask::LoadFromVideoTask(const tl::Path &video, 
                                     int skip,
                                     int videoIni,
                                     int videoEnd,
                                     const tl::Path &imagesPath,
                                     std::vector<Camera> *cameras,
                                     const std::string &cameraType)
  : tl::TaskBase(),
    mVideo(video),
    mSkipFrames(skip),
    mVideoIni(videoIni),
    mVideoEnd(videoEnd),
    mImagesPath(imagesPath),
    mCameras(cameras),
    mCameraType(cameraType)
{
}

LoadFromVideoTask::~LoadFromVideoTask()
{

}

void LoadFromVideoTask::execute(tl::Progress *progressBar)
{
  try {

    tl::Chrono chrono("Images loaded");
    chrono.run();

    ImportVideoFramesAlgorithm import_video_algorithm;
    import_video_algorithm.setVideo(mVideo);
    import_video_algorithm.setSkipFrames(mSkipFrames);
    import_video_algorithm.startAt(mVideoIni);
    import_video_algorithm.finishAt(mVideoEnd);

    if (!import_video_algorithm.open()) throw std::runtime_error("Open video error");
	
    int width = import_video_algorithm.width();
    int height = import_video_algorithm.height();
  
    Camera camera("Unknown camera", "");
    camera.setWidth(width);
    camera.setHeight(height);
    camera.setFocal(1.2 * std::max(width, height));
    int camera_id = mCameras->size();
    mCameras->push_back(camera);

    std::shared_ptr<tl::ImageMetadata> image_metadata = tl::ImageMetadataFactory::create("JPEG");
    image_metadata->setMetadata("EXIF_PixelXDimension", std::to_string(width));
    image_metadata->setMetadata("EXIF_PixelYDimension", std::to_string(height));

	  int delay = static_cast<int>(1000. / import_video_algorithm.framesPerSecond());
    char c;
    cv::Mat frame;
    while (!(frame = import_video_algorithm.read()).empty()) {

      if (status() == tl::Task::Status::stopping)  break;

      int pos = import_video_algorithm.framePosition();
      tl::Path path(mImagesPath);
      path.createDirectories();
      path.append(std::string("frame").append(std::to_string(pos)).append(".jpg"));
      Image image(QString::fromStdWString(path.toWString()));
      auto image_writer = tl::ImageWriterFactory::create(path);
      image_writer->open();
      TL_ASSERT(image_writer->isOpen(), "Can't create image");
      image_writer->setImageMetadata(image_metadata);
      image_writer->create(height, width, frame.channels(), tl::DataType::TL_8U);
      image_writer->write(frame);
      image_writer->close();
      
      emit image_added(image.path(), camera_id);

      if (progressBar) (*progressBar)();
    }
	
	
    if(status() == tl::Task::Status::stopping) {
      chrono.reset();
    } else {
      chrono.stop();
    }

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Load images error");
  }

}

} // graphos
