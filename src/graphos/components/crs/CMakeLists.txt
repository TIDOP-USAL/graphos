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

project(CoordinateReferenceSystem CXX)

set(CMAKE_DEBUG_POSTFIX d)
set (CMAKE_CXX_STANDARD 14)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

add_files_to_project(${PROJECT_NAME}
                     SOURCE_FILES
                         CoordinateReferenceSystemComponent.cpp
                         impl/CoordinateReferenceSystemModel.cpp
                         impl/CoordinateReferenceSystemView.cpp
                         impl/CoordinateReferenceSystemPresenter.cpp
                     HEADER_FILES
                         CoordinateReferenceSystemComponent.h
                         CoordinateReferenceSystemModel.h
                         CoordinateReferenceSystemView.h
                         CoordinateReferenceSystemPresenter.h
                         impl/CoordinateReferenceSystemModel.h
                         impl/CoordinateReferenceSystemView.h
                         impl/CoordinateReferenceSystemPresenter.h)

add_library(${PROJECT_NAME} ${LIB_TYPE}
            ${PROJECT_SOURCE_FILES}
            ${PROJECT_HEADER_FILES})

add_library(Graphos::${PROJECT_NAME} 
            ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE 
                      Qt5::Core
                      Qt5::Widgets
                      Graphos::Core
                      Graphos::Widgets
                      Eigen3::Eigen
                      glog::glog
                      Ceres::ceres
                      ${GDAL_LIBRARY} 
                      $<$<BOOL:${TL_HAVE_PROJ}>:PROJ::proj>)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Components")

install(TARGETS ${PROJECT_NAME}
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)
                    
install(DIRECTORY .
        DESTINATION include/graphos/components/${componentName}
        FILES_MATCHING PATTERN "*.h"
        PATTERN "impl" EXCLUDE)
		
set(GRAPHOS_HAVE_FEATEXTRACT YES CACHE INTERNAL "")

message(STATUS "  Graphos::CoordinateReferenceSystem enabled")
	
if(BUILD_TEST)
    #add_subdirectory(test)
endif(BUILD_TEST)