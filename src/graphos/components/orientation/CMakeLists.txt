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


if(BUILD_ORIENTATION_COMPONENT)

    project(Orientation LANGUAGES CXX)
    
    
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    # Instruct CMake to run moc automatically when needed
    set(CMAKE_AUTOMOC ON)
    # Create code from a list of Qt designer ui files
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    
    
    set(ORIENTATION_SOURCES
        OrientationComponent.cpp
        impl/OrientationModel.cpp
        impl/OrientationView.cpp
        impl/OrientationPresenter.cpp
        impl/OrientationCommand.cpp
        impl/ImportOrientationProcess.cpp)
        
    set(ORIENTATION_HEADERS
        OrientationComponent.h
        OrientationModel.h
        OrientationView.h
        OrientationPresenter.h
        impl/OrientationModel.h
        impl/OrientationView.h
        impl/OrientationPresenter.h
        impl/OrientationCommand.h
        impl/ImportOrientationProcess.h)
        
    add_library(${PROJECT_NAME} 
                ${ORIENTATION_SOURCES}
                ${ORIENTATION_HEADERS})

    add_library(Graphos::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

    foreach(_source IN ITEMS ${ORIENTATION_SOURCES})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "/" "\\" _group_path "${_source_path}")
        source_group("Source Files\\${_group_path}" FILES "${_source}")
    endforeach()
    
    foreach(_source IN ITEMS ${ORIENTATION_HEADERS})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "/" "\\" _group_path "${_source_path}")
        source_group("Header Files\\${_group_path}" FILES "${_source}")
    endforeach()
    
    target_link_libraries(${PROJECT_NAME} PRIVATE 
                          Qt5::Core
                          Qt5::Widgets
                          graphos_core
                          Eigen3::Eigen
                          glog::glog
                          Ceres::ceres)
    
    set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Components")
    
    install(TARGETS ${PROJECT_NAME}
            EXPORT "${TARGETS_EXPORT_NAME}"
            LIBRARY DESTINATION lib 
            ARCHIVE DESTINATION lib)
                        
    install(DIRECTORY .
            DESTINATION include/graphos/components/orientation
            FILES_MATCHING PATTERN "*.h"
            PATTERN "impl" EXCLUDE)
        
        
    set(GRAPHOS_HAVE_ORIENTATION YES CACHE INTERNAL "")
    
    message(STATUS "  Graphos::Orientation enabled")
    
else()

    message(STATUS "  Graphos::Orientation disabled")    
    
endif(BUILD_ORIENTATION_COMPONENT)