##########################################################################
#                                                                        #
# Copyright 2016 by Tidop Research Group <daguilera@usal.es>             #
#                                                                        #
# This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.       #
#                                                                        #
# GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can   #
# redistribute it and/or modify it under the terms of the GNU General    #
# Public License as published by the Free Software Foundation, either    #
# version 3 of the License, or (at your option) any later version.       #
#                                                                        #
# GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the       #
# hope that it will be useful, but WITHOUT ANY WARRANTY; without even    #
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR    #
# PURPOSE.  See the GNU General Public License for more details.         #
#                                                                        #
# You should have received a copy of the GNU General Public License      #
# along with Graphos.  If not, see <http://www.gnu.org/licenses/>.       #
#                                                                        #
# https://spdx.org/licenses/GPL-3.0-or-later.html                        #
#                                                                        #
##########################################################################

include(TidopUtils)

include_directories(${CMAKE_SOURCE_DIR}/third_party)


project(Core LANGUAGES CXX)

add_files_to_project(${PROJECT_NAME}
                     SOURCE_FILES
                         Application.cpp
                         AppStatus.cpp
                         ColorTable.cpp
                         Component.cpp
                         settings.cpp
                         Pdf.cpp
                         ply.cpp
                         utils.cpp
                         camera/Calibration.cpp
                         camera/Camera.cpp
                         camera/CameraRepository.cpp
                         camera/Colmap.cpp
                         camera/Database.cpp
                         camera/Undistort.cpp
                         camera/io/CalibrationReader.cpp
                         camera/io/CalibrationWriter.cpp
                         camera/io/impl/AgisoftCalibrationReader.cpp
                         camera/io/impl/AgisoftCalibrationWriter.cpp
                         camera/io/impl/OpenCVCalibrationReader.cpp
                         camera/io/impl/OpenCVCalibrationWriter.cpp
                         camera/io/impl/OpenDroneMapCalibrationReader.cpp
                         camera/io/impl/OpenDroneMapCalibrationWriter.cpp
                         camera/io/impl/Pix4DCalibrationReader.cpp
                         camera/io/impl/Pix4DCalibrationWriter.cpp
                         dense/CmvsPmvsProperties.cpp
                         dense/DensificationProperties.cpp
                         dense/MvsProperties.cpp
                         dense/SmvsProperties.cpp
                         dense/io/DenseExport.cpp
                         features/FeatureExtractorProperties.cpp
                         features/FeaturesRepository.cpp
                         features/SiftProperties.cpp
                         image/Image.cpp
                         image/ImageRepository.cpp
                         matching/MatchingProperties.cpp
                         matching/MatchingRepository.cpp
                         mesh/PoissonReconProperties.cpp
                         multispectral/Vignetting.cpp
                         orientation/CameraPose.cpp
                         orientation/CameraPosesRepository.cpp
                         orientation/ColmapReconstructionConvert.cpp
                         orientation/GroundControlPoint.cpp
                         orientation/GroundPoint.cpp
                         orientation/OrientationProperties.cpp
                         orientation/Track.cpp
                         orientation/io/CameraPosesReader.cpp
                         orientation/io/CameraPosesWriter.cpp
                         orientation/io/OrientationExport.cpp
                         project/Project.cpp
                         project/ProjectInfo.cpp
                         project/io/ProjectReader.cpp
                         project/io/ProjectWriter.cpp
                         task/Progress.cpp
                         task/TaskPresenter.cpp         
                         task/TaskView.cpp
                     HEADER_FILES
                         Application.h
                         AppStatus.h
                         ColorTable.h
                         command.h
                         Component.h
                         mvp.h
                         Pdf.h
                         ply.h
                         settings.h
                         utils.h
                         base/Repository.h
                         camera/Calibration.h
                         camera/Camera.h
                         camera/CameraRepository.h
                         camera/Colmap.h
                         camera/Database.h
                         camera/Undistort.h
                         camera/io/CalibrationReader.h
                         camera/io/CalibrationWriter.h
                         camera/io/impl/AgisoftCalibrationReader.h
                         camera/io/impl/AgisoftCalibrationWriter.h
                         camera/io/impl/OpenCVCalibrationReader.h
                         camera/io/impl/OpenCVCalibrationWriter.h
                         camera/io/impl/OpenDroneMapCalibrationReader.h
                         camera/io/impl/OpenDroneMapCalibrationWriter.h
                         camera/io/impl/Pix4DCalibrationReader.h
                         camera/io/impl/Pix4DCalibrationWriter.h
                         dem/DemReport.h
                         dense/CmvsPmvsProperties.h
                         dense/DensificationReport.h
                         dense/DensificationProperties.h
                         dense/MvsProperties.h
                         dense/SmvsProperties.h
                         dense/io/DenseExport.h
                         features/FeatureExtractorProperties.h
                         features/FeatureExtractorReport.h
                         features/FeaturesRepository.h
                         features/SiftProperties.h
                         image/Image.h
                         image/ImageRepository.h
                         matching/MatchingProperties.h
                         matching/MatchingReport.h
                         matching/MatchingRepository.h
                         mesh/MeshReport.h
                         mesh/PoissonReconProperties.h
                         multispectral/Vignetting.h
                         orientation/CameraPose.h
                         orientation/CameraPosesRepository.h
                         orientation/ColmapReconstructionConvert.h
                         orientation/GroundControlPoint.h
                         orientation/GroundPoint.h
                         orientation/OrientationProperties.h
                         orientation/OrientationReport.h
                         orientation/Track.h
                         orientation/io/CameraPosesReader.h
                         orientation/io/CameraPosesWriter.h
                         orientation/io/OrientationExport.h
                         ortho/OrthophotoReport.h
                         project/Project.h
                         project/ProjectInfo.h
                         project/io/ProjectReader.h
                         project/io/ProjectWriter.h
                         task/Progress.h
                         task/TaskPresenter.h            
                         task/TaskView.h)

	
add_library(${PROJECT_NAME} ${LIB_TYPE}
            ${PROJECT_SOURCE_FILES}
            ${PROJECT_HEADER_FILES})
            
add_library(Graphos::${PROJECT_NAME} 
            ALIAS ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${GRAPHOS_SOURCE_DIR}/src>
                           $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} PUBLIC
                      TidopLib::Core
                      TidopLib::Geometry
                      TidopLib::Geospatial
                      TidopLib::RasterTools
                      TidopLib::Math
                      TidopLib::VectorTools
                      TidopLib::Graphic
                      glog::glog
                      Qt6::PrintSupport
                      Qt6::Sql
                      Eigen3::Eigen
                      Ceres::ceres
                      colmap::colmap
                      ${CUDA_LIBRARIES}
                      ${CUDA_cublas_LIBRARY}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::glew
                      CGAL::CGAL
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      #[[${PIXKIT_LIBRARIES}]])


set_target_properties(${PROJECT_NAME} PROPERTIES 
                      FOLDER "lib"
                      OUTPUT_NAME "graphos_core")

install(TARGETS ${PROJECT_NAME}
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/core
        FILES_MATCHING PATTERN "*.h")

set(GRAPHOS_LIBRARIES ${GRAPHOS_LIBRARIES} ${PROJECT_NAME})
