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
                         Image.cpp
                         #project.cpp
                         settings.cpp
                         Pdf.cpp
                         ply.cpp
                         utils.cpp
                         camera/Calibration.cpp
                         camera/Camera.cpp
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
                         dense/dense.cpp
                         dense/CmvsPmvs.cpp
                         dense/Smvs.cpp
                         dense/mvs.cpp
                         dense/DenseExport.cpp
                         #features/featextract.cpp
                         #features/featio.cpp
                         features/Features.cpp
                         features/Sift.cpp
                         features/matching.cpp
                         io/ProjectReader.cpp
                         io/ProjectWriter.cpp
                         mesh/PoissonRecon.cpp
                         #multispectral/Radiance.cpp
                         #multispectral/Reflectance.cpp
                         multispectral/Vignetting.cpp
                         project/Project.cpp
                         project/ProjectInfo.cpp
                         repositories/CameraRepository.cpp
                         repositories/FeaturesRepository.cpp
                         repositories/ImageRepository.cpp
                         #sfm/orientationcolmap.cpp
                         #sfm/orientationexport.cpp
                         sfm/poses.cpp
                         #sfm/posesio.cpp
                         sfm/groundpoint.cpp
                         sfm/track.cpp
                         #sfm/BundleAdjuster.cpp
                         #sfm/Reconstruction.cpp
                         task/TaskPresenter.cpp         
                         task/TaskView.cpp
                         task/Progress.cpp
                         #[[ortho/Orthorectification.cpp
                         ortho/ZBuffer.cpp
                         ortho/Footprint.cpp
                         ortho/Orthoimage.cpp
                         preprocess/clahe.cpp
                         preprocess/dhe.cpp
                         preprocess/pohe.cpp]]
                     HEADER_FILES
                         Application.h
                         AppStatus.h
                         ColorTable.h
                         command.h
                         Component.h
                         Image.h
                         mvp.h
                         Pdf.h
                         ply.h
                         #project.h
                         settings.h
                         utils.h
                         camera/Calibration.h
                         camera/Camera.h
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
                         dense/dense.h
                         dense/CmvsPmvs.h
                         dense/Smvs.h
                         dense/mvs.h
                         dense/DenseExport.h
                         #features/featextract.h
                         features/Features.h
                         #features/featio.h
                         features/Sift.h
                         features/matching.h
                         io/ProjectReader.h
                         io/ProjectWriter.h
                         mesh/PoissonRecon.h
                         #multispectral/Radiance.h
                         #multispectral/Reflectance.h
                         multispectral/Vignetting.h
                         project/Project.h
                         project/ProjectInfo.h
                         reports/dem.h
                         reports/orthophoto.h
                         repositories/CameraRepository.h
                         repositories/FeaturesRepository.h
                         repositories/ImageRepository.h
                         repositories/Repository.h
                         #sfm/orientation.h
                         #sfm/orientationcolmap.h
                         #sfm/orientationexport.h
                         sfm/poses.h
                         #sfm/posesio.h
                         sfm/groundpoint.h
                         sfm/track.h
                         sfm/OrientationReport.h
                         #sfm/BundleAdjuster.h
                         #sfm/Reconstruction.h
                         task/TaskPresenter.h            
                         task/TaskView.h
                         task/Progress.h
                         #[[ortho/Orthorectification.h
                         ortho/ZBuffer.h
                         ortho/Footprint.h
                         ortho/Orthoimage.h
                         reports/dem.h
                         reports/orthophoto.h
                         #[[preprocess/preprocess.h
                         preprocess/clahe.h
                         preprocess/dhe.h
                         preprocess/pohe.h]])

	
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
