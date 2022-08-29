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

include_directories(${CMAKE_SOURCE_DIR}/third_party)

project(graphos_core LANGUAGES CXX)

set(GRAPHOS_CORE_SOURCES
    Application.cpp
    AppStatus.cpp
    Component.cpp
    project.cpp
    settings.cpp
    image.cpp
    Pdf.cpp
    process/ProcessPresenter.cpp         
    process/ProcessView.cpp
    process/Progress.cpp
    features/featio.cpp
    features/sift.cpp
    features/featextract.cpp
    features/matching.cpp
    sfm/orientationcolmap.cpp
    sfm/orientationexport.cpp
    sfm/poses.cpp
    sfm/posesio.cpp
    sfm/groundpoint.cpp
    sfm/track.cpp
    dense/dense.cpp
    dense/CmvsPmvs.cpp
    dense/Smvs.cpp
    dense/Mvs.cpp
    dense/DenseExport.cpp
    mesh/PoissonRecon.cpp
    utils.cpp
    dtm/dtm.cpp
    dtm/invdist.cpp
    dtm/invdistnn.cpp
    dtm/csf.cpp
    camera/Calibration.cpp
    camera/Camera.cpp
    camera/Colmap.cpp
    camera/Database.cpp
    camera/Undistort.cpp
    ortho/Orthorectification.cpp
    ortho/ZBuffer.cpp
    ortho/Footprint.cpp
    ortho/Orthoimage.cpp
    ortho/Orthomosaic.cpp)
			
set(GRAPHOS_CORE_HEADERS
    mvp.h
    Application.h
    AppStatus.h
    Component.h
    command.h
    project.h
    settings.h
    image.h
    Pdf.h
    process/ProcessPresenter.h            
    process/ProcessView.h
    process/Progress.h
    process/Task.h
    features/features.h
    features/featio.h
    features/sift.h
    features/featextract.h
    features/matching.h
    sfm/orientation.h
    sfm/orientationcolmap.h
    sfm/orientationexport.h
    sfm/poses.h
    sfm/posesio.h
    sfm/groundpoint.h
    sfm/track.h
    dense/dense.h
    dense/CmvsPmvs.h
    dense/Smvs.h
    dense/Mvs.h
    dense/DenseExport.h
    mesh/PoissonRecon.h
    utils.h
    dtm/dtm.h
    dtm/invdist.h
    dtm/invdistnn.h
    dtm/csf.h
    camera/Calibration.h
    camera/Camera.h
    camera/Colmap.h
    camera/Database.h
    camera/Undistort.h
    ortho/Orthorectification.h
    ortho/ZBuffer.h
    ortho/Footprint.h
    ortho/Orthoimage.h
    ortho/Orthomosaic.h)
	
add_library(${PROJECT_NAME} ${LIB_TYPE}
            ${GRAPHOS_CORE_SOURCES}
            ${GRAPHOS_CORE_HEADERS})
            
add_library(Graphos::Core ALIAS ${PROJECT_NAME})

foreach(_source IN ITEMS ${GRAPHOS_CORE_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Source Files\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${GRAPHOS_CORE_HEADERS})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Header Files\\${_group_path}" FILES "${_source}")
endforeach()


target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${GRAPHOS_SOURCE_DIR}/src>
                           $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} PUBLIC
                      TidopLib::tl_core
                      TidopLib::tl_geom
                      TidopLib::tl_geospatial
                      TidopLib::tl_img
                      TidopLib::tl_math
                      TidopLib::tl_vect
                      TidopLib::tl_graphic
                      CSF
                      glog::glog
                      Qt5::PrintSupport
                      Eigen3::Eigen
                      Ceres::ceres
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::glew
                      #${PROJ4_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      )

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "lib")

install(TARGETS ${PROJECT_NAME}
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/core
        FILES_MATCHING PATTERN "*.h")

set(GRAPHOS_LIBRARIES ${GRAPHOS_LIBRARIES} ${PROJECT_NAME})