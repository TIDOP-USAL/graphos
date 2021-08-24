/************************************************************************
 *                                                                      *
 *  Copyright 2016 by Tidop Research Group <daguilera@usal.se>          *
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
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.      *
 *                                                                      *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>                *
 *                                                                      *
 ************************************************************************/



#include "Application.h"

#include "graphos/core/AppStatus.h"
//#include "graphos/core/ImageManager.h"

namespace graphos
{

std::unique_ptr<Application> Application::sApplication;
std::mutex Application::sMutex;

Application::Application()
  : mAppStatus(new AppStatus())/* ,
    mImageManager(new ImageManager()) */
{
/*   connect(mImageManager, &ImageManager::imagesLoaded, [=](bool active) {
    mAppStatus->activeFlag(AppStatus::Flag::image_open, active);
  }); */
}

Application::~Application()
{
  if (mAppStatus != nullptr) {
    delete mAppStatus;
    mAppStatus = nullptr;
  }

/*   if (mImageManager != nullptr) {
    delete mImageManager;
    mImageManager = nullptr;
  } */

  sApplication.release();
}

AppStatus *Application::status()
{
  return mAppStatus;
}

/* ImageManager *Application::imageManager()
{
  return mImageManager;
} */

Application &Application::instance()
{
  if (sApplication.get() == nullptr) {
    std::lock_guard<std::mutex> lck(Application::sMutex);
    if (sApplication.get() == nullptr) {
      sApplication.reset(new Application());
    }
  }
  return *sApplication;
}

} // namespace graphos
