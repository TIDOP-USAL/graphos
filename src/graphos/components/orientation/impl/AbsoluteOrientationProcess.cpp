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

#include "AbsoluteOrientationProcess.h"

//#include "graphos/core/orientation/orientationcolmap.h"
#include "graphos/core/orientation/orientationexport.h"

#include <tidop/core/messages.h>
#include <tidop/core/chrono.h>

#include <colmap/base/reconstruction.h>

#include <fstream>

namespace graphos
{

constexpr auto min_common_images = 3;
constexpr auto robust_alignment = true;
constexpr auto robust_alignment_max_error = 1.;

AbsoluteOrientationProcess::AbsoluteOrientationProcess(const QString &inputPath,
                                                       const std::map<QString, std::array<double, 3>> &cameraPositions,
                                                       const QString &outputPath)
  : tl::TaskBase(),
    mMinCommonImages(min_common_images),
    mRobustAlignment(robust_alignment),
    mRobustAlignmentMaxError(robust_alignment_max_error),
    mInputPath(inputPath),
    mCameraPositions(cameraPositions),
    mOutputPath(outputPath)
{

}

AbsoluteOrientationProcess::~AbsoluteOrientationProcess()
{
}

void AbsoluteOrientationProcess::stop()
{
  TaskBase::stop();

}

void AbsoluteOrientationProcess::execute(tl::Progress *progressBar)
{
  try {

    msgInfo("Starting Absolute Orientation");

    tl::Chrono chrono("Absolute Orientation process finished");
    chrono.run();

    bool robust_alignment = mRobustAlignment;
    colmap::RANSACOptions ransac_options;
    ransac_options.max_error = mRobustAlignmentMaxError;
    int min_common_images = mMinCommonImages;

    tl::Path dir(mOutputPath.toStdString());
    if(!dir.exists() && !dir.createDirectories()) {
      throw std::runtime_error(std::string("Directory couldn't be created: ").append(mOutputPath.toStdString()));
    }

    if(robust_alignment && ransac_options.max_error <= 0) {
      throw std::runtime_error("ERROR: You must provide a maximum alignment error > 0");
    }

    std::vector<std::string> ref_image_names;
    std::vector<Eigen::Vector3d> ref_locations;
    //std::vector<std::string> lines = colmap::ReadTextFileLines(mImagePath.toStdString());
    //for (const auto line : lines) {
    //  std::stringstream line_parser(line);
    //  std::string image_name = "";
    //  Eigen::Vector3d camera_position;
    //  line_parser >> image_name >> camera_position[0] >> camera_position[1] >>
    //    camera_position[2];
    //  ref_image_names.push_back(image_name);
    //  ref_locations.push_back(camera_position);
    //}

    Eigen::Vector3d offset(0., 0., 0.);

    double i = 1.;
    for(const auto &cameraPosition : mCameraPositions) {
      std::string image_name = cameraPosition.first.toStdString();
      ref_image_names.push_back(image_name);
      Eigen::Vector3d camera_position;
      camera_position[0] = cameraPosition.second[0];
      camera_position[1] = cameraPosition.second[1];
      camera_position[2] = cameraPosition.second[2];

      //Para evitar desbordamiento
      offset += (camera_position - offset) / (i);
      ref_locations.push_back(camera_position);
      i++;
    }

    if(status() == tl::Task::Status::stopping) return;

    QString offset_file = mOutputPath + "/offset.txt";
    std::ofstream stream(offset_file.toStdString(), std::ios::trunc);
    if(stream.is_open()) {
      stream << QString::number(offset[0], 'f', 6).toStdString() << " "
        << QString::number(offset[1], 'f', 6).toStdString() << " "
        << QString::number(offset[2], 'f', 6).toStdString() << std::endl;

      msgInfo("Camera offset: %lf,%lf,%lf", offset[0], offset[1], offset[2]);

      stream.close();
    }

    for(auto &pos : ref_locations) {
      pos -= offset;
    }

    if(!tl::Path(mInputPath.toStdString()).exists())
      throw std::runtime_error(std::string("Reconstruction not found in path: ").append(mInputPath.toStdString()));

    colmap::Reconstruction reconstruction;
    reconstruction.Read(mInputPath.toStdString());

    if(status() == tl::Task::Status::stopping) return;

    bool alignment_success;
    if(robust_alignment) {
      alignment_success = reconstruction.AlignRobust(
        ref_image_names, ref_locations, min_common_images, ransac_options);
    } else {
      alignment_success =
        reconstruction.Align(ref_image_names, ref_locations, min_common_images);
    }

    if(status() == tl::Task::Status::stopping) return;

    if(!alignment_success) throw std::runtime_error("Absolute Orientation failed");

    reconstruction.Write(mOutputPath.toStdString());

    if(status() == tl::Task::Status::stopping) return;

    OrientationExport orientationExport(&reconstruction);
    orientationExport.exportPLY(mOutputPath + "/sparse.ply");

    std::vector<double> errors;
    errors.reserve(ref_image_names.size());

    for(size_t i = 0; i < ref_image_names.size(); ++i) {
      const colmap::Image *image = reconstruction.FindImageWithName(ref_image_names[i]);
      if(image != nullptr) {
        double error = (image->ProjectionCenter() - ref_locations[i]).norm();
        std::cout << image->Name() << "[ Error: " << error << std::endl;
        errors.push_back(error);
      }
    }

    msgInfo("Alignment error: %f (mean), %f (median)",
            colmap::Mean(errors), colmap::Median(errors));

    if(status() == tl::Task::Status::stopping) return;

    chrono.stop();

    if(progressBar) (*progressBar)();

    emit absoluteOrientationFinished();

  } catch(...) {
    TL_THROW_EXCEPTION_WITH_NESTED("Absolute Orientation error");
  }
}

} // namespace graphos