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

#ifndef GRAPHOS_CORE_DENSE_MVS_H
#define GRAPHOS_CORE_DENSE_MVS_H

#include "config_graphos.h"

#include "graphos/core/densification/densification.h"

#include <QString>

namespace graphos
{

class ReadCalibration;

namespace internal
{
class Reconstruction;
}

class GRAPHOS_EXPORT MvsProperties
  : public Mvs
{

public:

  MvsProperties();
  MvsProperties(const MvsProperties &mvs);
  MvsProperties(MvsProperties &&mvs) noexcept;
  ~MvsProperties() override = default;

  MvsProperties &operator =(const MvsProperties &mvs);
  MvsProperties &operator =(MvsProperties &&mvs) noexcept;

// Mvs interface

public:


// Densification interface

public:

  void reset() override;
  QString name() const override;

protected:


};


/*----------------------------------------------------------------*/


class GRAPHOS_EXPORT MvsDensifier
  : public MvsProperties,
    public Densifier
{

public:

  MvsDensifier();
  MvsDensifier(bool cuda);
  ~MvsDensifier() override;
  
  MvsDensifier(const MvsDensifier &MvsProcess) = delete;
  MvsDensifier(MvsDensifier &&MvsProcess) = delete;
  MvsDensifier &operator =(const MvsDensifier &MvsProcess) = delete;
  MvsDensifier &operator =(MvsDensifier &&MvsProcess) = delete;

// DensificationProcess interface

  bool undistort(const QString &reconstructionPath,
                 const QString &outputPath) override;
  bool densify(const QString &undistortPath) override;
  void enableCuda(bool enable) override;

// Densification interface

public:

  void reset() override;

// Member functions

private:

  void createDirectories();
  void createDirectory(const std::string &path);
  void writeNVMFile();
  void undistortImages();
  void undistortImage();
  void exportToMVS();

// Data members

private:

  bool bOpenCvRead;
  bool bCuda;
  std::string mOutputPath;
  internal::Reconstruction *mReconstruction;
  ReadCalibration *mCalibrationReader;
};


} // namespace graphos

#endif // GRAPHOS_CORE_DENSE_MVS_H