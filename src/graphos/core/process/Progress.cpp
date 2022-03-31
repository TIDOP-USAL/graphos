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

#include "graphos/core/process/Progress.h"

namespace graphos
{

ProgressHandler::ProgressHandler(QObject *parent)
  : QObject(parent),
    tl::ProgressBase()
{
  //emit valueChange(0);
  //emit initialized();
}

ProgressHandler::ProgressHandler(size_t min, 
                                 size_t max, 
                                 QObject *parent)
  : QObject(parent),
    tl::ProgressBase(min, max)
{
  //emit valueChange(0);
  //emit initialized();
}

ProgressHandler::~ProgressHandler()
{

}

//int ProgressHandler::min() const
//{
//  return mMin;
//}
//
//int ProgressHandler::max() const
//{
//  return mMax;
//}

//int ProgressHandler::value() const
//{
//  return mValue;
//}

//void ProgressHandler::setValue(int value)
//{
//  mValue = value;
//  emit valueChange(value);
//}

//void ProgressHandler::next()
//{
//  setValue(++mValue);
//}

void ProgressHandler::init()
{
  emit valueChange(0);
  emit initialized();
}

void ProgressHandler::finish()
{
  emit finished();
}

void ProgressHandler::setTitle(const QString &title)
{
  emit titleChange(title);
}

void ProgressHandler::setDescription(const QString &description)
{
  emit descriptionChange(description);
}

void ProgressHandler::setCloseAuto(bool active)
{
  emit closeAuto(active);
}

void ProgressHandler::setRange(size_t min, size_t max)
{
  ProgressBase::setRange(min, max);
  emit valueChange(0);
  if (min == 0 && max == 1)
    emit rangeChange(0, 0);
  else 
    emit rangeChange(0, 100);
  //emit initialized();
}

void ProgressHandler::updateProgress()
{
  emit valueChange(percent());
}

void ProgressHandler::terminate()
{
  if(minimun() == 0 && maximun() == 1) {
    emit rangeChange(0, 100);
    emit valueChange(100);
  }
  emit finished();
}

} // namespace graphos


