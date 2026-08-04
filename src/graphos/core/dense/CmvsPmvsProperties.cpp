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

#include "graphos/core/dense/CmvsPmvsProperties.h"

using namespace tl;

namespace graphos
{

CmvsPmvsProperties::CmvsPmvsProperties()
  : DensificationProperties("CMVS/PMVS")
{
    clear();
}

auto CmvsPmvsProperties::useVisibilityInformation() const -> bool
{
    return mProperties.getProperty<bool>("UseVisibilityInformation");
}

auto CmvsPmvsProperties::imagesPerCluster() const -> int
{
    return mProperties.getProperty<int>("ImagesPerCluster");
}

auto CmvsPmvsProperties::level() const -> int
{
    return mProperties.getProperty<int>("Level");
}

auto CmvsPmvsProperties::cellSize() const -> int
{
    return mProperties.getProperty<int>("CellSize");
}

auto CmvsPmvsProperties::threshold() const -> double
{
    return mProperties.getProperty<double>("Threshold");
}

auto CmvsPmvsProperties::windowSize() const -> int
{
    return mProperties.getProperty<int>("WindowSize");
}

auto CmvsPmvsProperties::minimunImageNumber() const -> int
{
    return mProperties.getProperty<int>("MinimunImageNumber");
}

void CmvsPmvsProperties::setUseVisibilityInformation(bool useVisibilityInformation)
{
    mProperties.setProperty("UseVisibilityInformation", useVisibilityInformation);
}

void CmvsPmvsProperties::setImagesPerCluster(int imagesPerCluster)
{
    mProperties.setProperty("ImagesPerCluster", imagesPerCluster);
}

void CmvsPmvsProperties::setLevel(int level)
{
    mProperties.setProperty("Level", level);
}

void CmvsPmvsProperties::setCellSize(int cellSize)
{
    mProperties.setProperty("CellSize", cellSize);
}

void CmvsPmvsProperties::setThreshold(double threshold)
{
    mProperties.setProperty("Threshold", threshold);
}

void CmvsPmvsProperties::setWindowSize(int windowSize)
{
    mProperties.setProperty("WindowSize", windowSize);
}

void CmvsPmvsProperties::setMinimunImageNumber(int minimunImageNumber)
{
    mProperties.setProperty("MinimunImageNumber", minimunImageNumber);
}

void CmvsPmvsProperties::clear()
{
    setUseVisibilityInformation(true);
    setImagesPerCluster(100);
    setLevel(1);
    setCellSize(2);
    setThreshold(0.7);
    setWindowSize(7);
    setMinimunImageNumber(3);
}

} // namespace graphos
