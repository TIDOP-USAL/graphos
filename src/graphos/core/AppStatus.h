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

#ifndef GRAPHOS_APP_STATUS_H
#define GRAPHOS_APP_STATUS_H

#include "graphos/graphos_global.h"

#include <tidop/core/flags.h>

#include <QObject>

#include <memory>


namespace graphos
{

class AppStatus
  : public QObject
{

  Q_OBJECT

public:

  enum class Flag : uint32_t
  {
    none                  = (0 << 0),
    project_exists        = (1 << 0),  // Existe un proyecto
    project_modified      = (1 << 1),  // Se ha modificado el proyecto
    images_added          = (1 << 2),  // Se han añadido fotogramas
    image_open            = (1 << 3),  // Hay una imagen abierta
    feature_extraction    = (1 << 4),
    feature_matching      = (1 << 5),
    oriented              = (1 << 6),
    absolute_oriented     = (1 << 7),
    dense_model           = (1 << 8),
    dtm                   = (1 << 9),
    ortho                 = (1 << 10),
    mesh                  = (1 << 11),
    processing            = (1 << 20),
    loading_images        = (1 << 21),
    tab_image_active      = (1 << 22),
    tab_3d_model_active   = (1 << 23),
    loading_thumbs        = (1 << 24),
    command_mode          = (1 << 30)
  };

public:

  AppStatus();
  ~AppStatus();

  AppStatus(const AppStatus &) = delete;
  AppStatus(AppStatus &&) = delete;
  AppStatus operator=(const AppStatus &) = delete;
  AppStatus operator=(AppStatus &&) = delete;

  /* Graphos Flags */

  void activeFlag(Flag flag, bool active);
  bool isEnabled(Flag flag) const;
  void enable(Flag flag);
  void disable(Flag flag);
  void switchFlag(Flag flag);

  /* User flags */
  
  bool isEnabled(uint32_t flag) const;
  void enable(uint32_t flag);
  void disable(uint32_t flag);
  void switchFlag(uint32_t flag);

  void clear();

signals:

  void update();

private:

  tl::EnumFlags<Flag> mFlags;
  tl::Flags<uint32_t> mUserFlags;
  
};
ALLOW_BITWISE_FLAG_OPERATIONS(AppStatus::Flag)

} // namespace graphos


#endif // GRAPHOS_APP_STATUS_H
