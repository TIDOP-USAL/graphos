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
                         Component.cpp
                         project.cpp
                         settings.cpp
                         image.cpp
                         Pdf.cpp
                         ColorTable.cpp
                         ply.cpp
                         task/TaskPresenter.cpp         
                         task/TaskView.cpp
                         task/Progress.cpp
                         features/featextract.cpp
                         features/featio.cpp
                         features/sift.cpp
                         features/matching.cpp
                         sfm/orientationcolmap.cpp
                         sfm/orientationexport.cpp
                         sfm/poses.cpp
                         sfm/posesio.cpp
                         sfm/groundpoint.cpp
                         sfm/track.cpp
                         sfm/BundleAdjuster.cpp
                         sfm/Reconstruction.cpp
                         dense/dense.cpp
                         dense/CmvsPmvs.cpp
                         dense/Smvs.cpp
                         dense/mvs.cpp
                         dense/DenseExport.cpp
                         mesh/PoissonRecon.cpp
                         utils.cpp
                         camera/Calibration.cpp
                         camera/Camera.cpp
                         camera/Colmap.cpp
                         camera/Database.cpp
                         camera/Undistort.cpp
                         ortho/Orthorectification.cpp
                         ortho/ZBuffer.cpp
                         ortho/Footprint.cpp
                         ortho/Orthoimage.cpp
                         #[[preprocess/clahe.cpp
                         preprocess/dhe.cpp
                         preprocess/pohe.cpp]]
                     HEADER_FILES
                         mvp.h
                         Application.h
                         AppStatus.h
                         Component.h
                         command.h
                         project.h
                         settings.h
                         image.h
                         Pdf.h
                         ColorTable.h
                         ply.h
                         task/TaskPresenter.h            
                         task/TaskView.h
                         task/Progress.h
                         features/featextract.h
                         features/features.h
                         features/featio.h
                         features/sift.h
                         features/matching.h
                         sfm/orientation.h
                         sfm/orientationcolmap.h
                         sfm/orientationexport.h
                         sfm/poses.h
                         sfm/posesio.h
                         sfm/groundpoint.h
                         sfm/track.h
                         sfm/OrientationReport.h
                         sfm/BundleAdjuster.h
                         sfm/Reconstruction.h
                         dense/dense.h
                         dense/CmvsPmvs.h
                         dense/Smvs.h
                         dense/mvs.h
                         dense/DenseExport.h
                         mesh/PoissonRecon.h
                         utils.h
                         camera/Calibration.h
                         camera/Camera.h
                         camera/Colmap.h
                         camera/Database.h
                         camera/Undistort.h
                         ortho/Orthorectification.h
                         ortho/ZBuffer.h
                         ortho/Footprint.h
                         ortho/Orthoimage.h
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
                      TidopLib::Geom
                      TidopLib::Geospatial
                      TidopLib::Img
                      TidopLib::Math
                      TidopLib::Vect
                      TidopLib::Graphic
                      glog::glog
                      Qt5::PrintSupport
                      Qt5::Sql
                      Eigen3::Eigen
                      Ceres::ceres
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${CUDA_cublas_LIBRARY}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::glew
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      CGAL::CGAL
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
