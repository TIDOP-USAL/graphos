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
 * along with Graphos.  If not, see <http://www.gnu.org/licenses/>.     *
 *                                                                      *
 * https://spdx.org/licenses/GPL-3.0-or-later.html                      *
 *                                                                      *
 ************************************************************************/

#include "graphos/core/ortho/Footprint.h"

#include "graphos/core/ortho/Orthorectification.h"

#include <tidop/graphic/layer.h>
#include <tidop/graphic/entities/polygon.h>
#include <tidop/graphic/datamodel.h>

namespace graphos
{

Footprint::Footprint(const std::vector<Image> &images,
                     const std::map<int,Camera> &cameras,
                     const tl::Path &dtm,
                     const tl::geospatial::Crs &crs,
                     const tl::Path &footprint)
  : mImages(images),
    mCameras(cameras),
    mDtm(dtm),
    mCrs(crs)
{
  footprint.parentPath().createDirectories();

  mFootprintWriter = tl::VectorWriterFactory::createWriter(footprint);
  mFootprintWriter->open();
}

Footprint::~Footprint()
{
}

void Footprint::execute(tl::Progress *progressBar)
{
  try {

    if (!mFootprintWriter->isOpen()) throw std::runtime_error("Vector open error");

    mFootprintWriter->create();
    mFootprintWriter->setCRS(mCrs.toWktFormat());

    std::shared_ptr<tl::TableField> field = std::make_shared<tl::TableField>("image", tl::TableField::Type::STRING, 254);
    tl::graph::GLayer layer;
    layer.setName("footprint");
    layer.addDataField(field);

    for (const auto &image : mImages) {

      Orthorectification orthorectification(mDtm, mCameras[image.cameraId()], image.cameraPose());
      std::shared_ptr<tl::graph::GPolygon> entity = std::make_shared<tl::graph::GPolygon>(orthorectification.footprint());
      std::shared_ptr<tl::TableRegister> data = std::make_shared <tl::TableRegister>(layer.tableFields());
      data->setValue(0, image.name().toStdString());
      entity->setData(data);
      layer.push_back(entity);

    }

    mFootprintWriter->write(layer);
    mFootprintWriter->close();

  } catch (...) {
    TL_THROW_EXCEPTION_WITH_NESTED("");
  }

}

} // namespace graphos