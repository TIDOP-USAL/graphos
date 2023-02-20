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

#include "graphos/core/dtm/csf.h"

#include <tidop/core/exception.h>

TL_SUPPRESS_WARNINGS
#include <colmap/util/ply.h>
#include <FileIOFilter.h>
#include <ccPointCloud.h>
#include "csf/src/CSF.h"
TL_DEFAULT_WARNINGS

namespace graphos
{

namespace internal
{

class Csf
  : private CSF
{
public:

  Csf()
  {
    setDefaultConfig();
  }

  ~Csf()
  {
  }

  bool sloopSmooth() const
  {
    return this->params.bSloopSmooth;
  }

  void setSloopSmooth(bool sloopSmooth)
  {
    this->params.bSloopSmooth = sloopSmooth;
  }

  double timeStep() const
  {
    return this->params.time_step;
  }

  void setTimeStep(double timeStep)
  {
    this->params.time_step = timeStep;
  }

  double classThreshold() const
  {
    return this->params.class_threshold;
  }

  void setClassThreshold(double classThreshold)
  {
    this->params.class_threshold = classThreshold;
  }

  double clothResolution() const
  {
    return this->params.cloth_resolution;
  }

  void setClothResolution(double clothResolution)
  {
    this->params.cloth_resolution = clothResolution;
  }

  int rigidness() const
  {
    return this->params.rigidness;
  }

  void setRigidness(int rigidness)
  {
    this->params.rigidness = rigidness;
  }

  int iterations() const
  {
    return this->params.interations;
  }

  void setIterations(int iterations)
  {
    this->params.interations = iterations;
  }

  void setPointCloud(csf::PointCloud &pointCloud)
  {
    csf::PointCloud &ref_point_cloud = this->getPointCloud();
    ref_point_cloud = pointCloud;
  }

  void filter(std::vector<int> &groundIdx,
              std::vector<int> &offGroundIdx)
  {
    this->do_filtering(groundIdx, offGroundIdx);
  }

  void setDefaultConfig()
  {
    //this->params.k_nearest_points = 1; //CloudCompare
    this->params.bSloopSmooth = false;
    this->params.time_step = 0.65;
    this->params.class_threshold = 0.5;
    this->params.cloth_resolution = 2;
    this->params.rigidness = 2;
    this->params.interations = 500;
  }

};

}




Csf::Csf()
   : mCSF(new internal::Csf())
{

}

Csf::~Csf()
{

}

bool Csf::sloopSmooth() const
{
  return mCSF->sloopSmooth();
}

void Csf::setSloopSmooth(bool sloopSmooth)
{
  mCSF->setSloopSmooth(sloopSmooth);
}

double Csf::timeStep() const
{
  return mCSF->timeStep();
}

void Csf::setTimeStep(double timeStep)
{
  mCSF->setTimeStep(timeStep);
}

double Csf::classThreshold() const
{
  return mCSF->classThreshold();
}

void Csf::setClassThreshold(double classThreshold)
{
  mCSF->setClassThreshold(classThreshold);
}

double Csf::clothResolution() const
{
  return mCSF->clothResolution();
}

void Csf::setClothResolution(double clothResolution)
{
  mCSF->setClothResolution(clothResolution);
}

int Csf::rigidness() const
{
  return mCSF->rigidness();
}

void Csf::setRigidness(int rigidness)
{
  mCSF->setRigidness(rigidness);
}

int Csf::iterations() const
{
  return mCSF->iterations();
}

void Csf::setIterations(int iterations)
{
  mCSF->setIterations(iterations);
}

void Csf::filter(const std::string &pointCloud,
                 const std::string &ground,
                 const std::string &outGround)
{
  try {

    /// TODO: Error al leer algunos ficheros ply
    /// Crear clase para lectura y escritura de ficheros PLY y no depender de terceros
    //std::vector<colmap::PlyPoint> points = colmap::ReadPly(pointCloud);


    FileIOFilter::LoadParameters parameters;
    parameters.alwaysDisplayLoadDialog = false;
    //parameters.parentWidget = this;
    CC_FILE_ERROR error = CC_FERR_NO_ERROR;
    ccHObject *cc_object = FileIOFilter::LoadFromFile(QString::fromStdString(pointCloud),
                                                        parameters,
                                                        error,
                                                        "PLY mesh (*.ply)");
    TL_ASSERT(error == CC_FERR_NO_ERROR, "Load file error");

    ccHObject::Container clouds;
    cc_object->filterChildren(clouds, true, CC_TYPES::POINT_CLOUD);

    if (clouds.size() != 1) return;

    auto cloud = static_cast<ccPointCloud *>(clouds.at(0));
    if (cloud == nullptr) return;


    
    csf::PointCloud csf_points;
    //csf_points.resize(points.size());
    csf_points.resize(cloud->size());

    for (size_t i = 0; i < cloud->size(); i++) {
      auto point = cloud->getPoint(i);
      //csf_points[i].x = static_cast<double>(points[i].x);
      //csf_points[i].y = static_cast<double>(-points[i].z);
      //csf_points[i].z = static_cast<double>(points[i].y);
      csf_points[i].x = static_cast<double>(point->x);
      csf_points[i].y = static_cast<double>(point->y);
      csf_points[i].z = static_cast<double>(point->z);
    }

    mCSF->setPointCloud(csf_points);

    std::vector<int> ground_idx;
    std::vector<int> off_ground_idx;
    mCSF->filter(ground_idx, off_ground_idx);

    /// Guardar nubes de puntos segmentadas
    std::vector<colmap::PlyPoint> points_ground;
    std::vector<colmap::PlyPoint> points_off_ground;
    points_ground.reserve(ground_idx.size());
    points_off_ground.reserve(off_ground_idx.size());

    for (const auto &idx : ground_idx) {
      colmap::PlyPoint point_colmap;
      point_colmap.x = csf_points[idx].x;
      point_colmap.y = csf_points[idx].y;
      point_colmap.z = csf_points[idx].z;
      //points_ground.push_back(points[idx]);
      points_ground.push_back(point_colmap);
    }

    for (const auto &idx : off_ground_idx) {
      //points_off_ground.push_back(points[idx]);
      colmap::PlyPoint point_colmap;
      point_colmap.x = csf_points[idx].x;
      point_colmap.y = csf_points[idx].y;
      point_colmap.z = csf_points[idx].z;
      points_off_ground.push_back(point_colmap);
    }

    colmap::WriteBinaryPlyPoints(ground, points_ground);
    colmap::WriteBinaryPlyPoints(outGround, points_off_ground);

  } catch (const std::bad_alloc &) {
    //m_app->dispToConsole("Not enough memory!", ccMainAppInterface::ERR_CONSOLE_MESSAGE);
  }
}

void Csf::reset()
{
  mCSF->setDefaultConfig();
}

} // namespace graphos
