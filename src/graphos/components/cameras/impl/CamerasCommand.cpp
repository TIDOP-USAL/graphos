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

#include "CamerasCommand.h"

#include <tidop/core/app/Logger.h>
#include <tidop/core/base/Exception.h>
#include <tidop/core/console/ValuesValidator.h>

#include "graphos/core/project/Project.h"
#include "graphos/core/project/io/ProjectReader.h"
#include "graphos/core/project/io/ProjectWriter.h"
#include "graphos/core/camera/io/CalibrationReader.h"
#include "graphos/core/camera/io/CalibrationWriter.h"

namespace graphos
{

CamerasCommand::CamerasCommand()
  : Command("cameras", "Cameras manager")
{
    auto prj = tl::Argument::make<tl::Path>("prj", 'p', "Path to the project file");
    auto list_cameras = tl::Argument::make<bool>("list", 'l', "List project cameras", false);
    auto camera_id = tl::Argument::make<int>("camera_id", 'c', "Show camera info", 0);
    auto export_calib = tl::Argument::make<tl::Path>("export_calib", "Calibration export file", tl::Path());
    auto import_calib = tl::Argument::make<tl::Path>("import_calib", "Calibration import file", tl::Path());
    std::vector<std::string> formats{"OpenCV", "Pix4D", "Agisoft", "ODM"};
    auto format_arg = tl::Argument::make<std::string>("format", 'f', "Calibration file format", formats.at(0));
    format_arg->setValidator(std::make_shared<tl::ValuesValidator<std::string>>(formats));

    this->addArgument(prj);
    this->addArgument(list_cameras);
    this->addArgument(camera_id);
    this->addArgument(export_calib);
    this->addArgument(import_calib);
    this->addArgument(format_arg);

    this->addUsage(tl::UsageSignature({prj, list_cameras}, {}, "List cameras"));
    this->addUsage(tl::UsageSignature({prj, camera_id}, {}, "Show camera ID"));
    this->addUsage(tl::UsageSignature({prj, camera_id, export_calib}, {format_arg}, "Export camera calibration"));
    this->addUsage(tl::UsageSignature({prj, camera_id, import_calib}, {format_arg}, "Import camera calibration"));

    this->addExample("cameras -p 253/253.xml --list");
    this->addExample("cameras -p 253/253.xml --camera 1");
    this->addExample("cameras -p 253/253.xml --camera 1 --export_calib calib.xml --format OpenCV");
    this->addExample("cameras -p 253/253.xml --camera 1 --import_calib calib.xml --format Agisoft");
}

CamerasCommand::~CamerasCommand()
{
}

bool CamerasCommand::run()
{
    bool r = false;

    auto &log = tl::Logger::instance();

    try {

        auto project_path = this->value<tl::Path>("prj");
        auto list_cameras = this->value<bool>("list");
        auto camera_id = this->value<int>("camera_id");
        auto export_calib = this->value<tl::Path>("export_calib");
        auto import_calib = this->value<tl::Path>("import_calib");
        auto format = this->value<std::string>("format");

        tl::Path log_path = project_path;
        log_path.replaceExtension(".log");
        log.open(log_path);

        TL_ASSERT(project_path.exists(), "Project doesn't exist");
        TL_ASSERT(project_path.isFile(), "Project file doesn't exist");

        Project project;
        ProjectReader reader;
        reader.read(project_path, project);

        auto &cameras = project.cameras();

        TL_ASSERT(!cameras.empty(), "No cameras in the project");

        if (list_cameras) {
            tl::Message::info("Cameras in the project:");
            for (const auto &[camera_id, camera] : cameras) {
                tl::Message::info("ID: {}, Make: {}, Model: {}", camera_id, camera.make(), camera.model());
            }
        } else {

            auto camera = cameras.find(camera_id);

            TL_ASSERT(camera, "Camera with ID {} not found", camera_id);

            if (!export_calib.empty()) {

                try {
                    auto calibration_writer = CalibrationWriterFactory::create(format);
                    calibration_writer->write(export_calib, *camera);
                } catch (...) {
                    TL_THROW_EXCEPTION_WITH_NESTED("Failed to export calibration to file: {}", export_calib.toUtf8());
                }

                tl::Message::info("Calibration exported to '{}'", export_calib.toUtf8());

            } else if (import_calib.exists() && import_calib.isFile()) {

                try {
                    auto calibration_reader = CalibrationReaderFactory::create(format);
                    calibration_reader->read(import_calib, *camera);
                } catch (...) {
                    TL_THROW_EXCEPTION_WITH_NESTED("Failed to import calibration to file: {}", import_calib.toUtf8());
                }

                tl::Message::info("Calibration imported from '{}'", import_calib.toUtf8());

            } else {
                tl::Message::info("Camera Make: {}", camera->make());
                tl::Message::info("Camera Model: {}", camera->model());
                tl::Message::info("Type: {}", camera->type());
                tl::Message::info("Focal length: {}", camera->focal());
                tl::Message::info("Sensor width: {} px", camera->width());
                tl::Message::info("Sensor height: {} px", camera->height());
                tl::Message::info("Sensor size: {} mm", camera->sensorSize());
            }

        }

        ProjectWriter writer;
        writer.write(project_path, project);

    } catch (const std::exception &e) {
        tl::printException(e);
        r = true;
    }

    log.close();

    return r;
}


} // namespace graphos
