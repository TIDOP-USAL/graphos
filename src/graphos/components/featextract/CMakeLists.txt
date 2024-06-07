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

if(BUILD_FEATEXTRACT_COMPONENT)

    project(FeatExtract LANGUAGES CXX)
    
    
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    # Instruct CMake to run moc automatically when needed
    set(CMAKE_AUTOMOC ON)
    # Create code from a list of Qt designer ui files
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    

    add_files_to_project(${PROJECT_NAME}
                         SOURCE_FILES
                             FeatureExtractorComponent.cpp
                             impl/FeatureExtractorCommand.cpp
                         HEADER_FILES
                             FeatureExtractorComponent.h
                             impl/FeatureExtractorCommand.h)

if (BUILD_GUI)        
    add_files_to_project(${PROJECT_NAME}
                         SOURCE_FILES
                             impl/FeatureExtractorView.cpp
                             impl/FeatureExtractorModel.cpp
                             impl/FeatureExtractorPresenter.cpp
                         HEADER_FILES
                             FeatureExtractorView.h
                             FeatureExtractorModel.h
                             FeatureExtractorPresenter.h
                             impl/FeatureExtractorView.h
                             impl/FeatureExtractorModel.h
                             impl/FeatureExtractorPresenter.h)
endif(BUILD_GUI)

    add_library(${PROJECT_NAME} ${LIB_TYPE}
                ${PROJECT_SOURCE_FILES}
                ${PROJECT_HEADER_FILES})
                
    add_library(Graphos::${PROJECT_NAME} 
                ALIAS ${PROJECT_NAME})
    
    target_link_libraries(${PROJECT_NAME} PRIVATE 
                          Qt5::Core
                          $<$<BOOL:${GRAPHOS_GUI}>:Qt5::Widgets>
                          Graphos::Core
                          $<$<BOOL:${GRAPHOS_GUI}>:Graphos::Widgets>
                          Eigen3::Eigen
                          glog::glog
                          Ceres::ceres)
    
    set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Components")
    
    install(TARGETS ${PROJECT_NAME}
            EXPORT "${TARGETS_EXPORT_NAME}"
            LIBRARY DESTINATION lib 
            ARCHIVE DESTINATION lib)
                        
    install(DIRECTORY .
            DESTINATION include/graphos/components/featextract
            FILES_MATCHING PATTERN "*.h"
            PATTERN "impl" EXCLUDE
            PATTERN "test" EXCLUDE)
        
        
    set(GRAPHOS_HAVE_FEATEXTRACT YES CACHE INTERNAL "")

    message(STATUS "  Graphos::FeatExtract enabled")
	
    if(BUILD_TEST)
        add_subdirectory(test)
    endif(BUILD_TEST)
	
else()

    message(STATUS "  Graphos::FeatExtract disabled")
    
endif(BUILD_FEATEXTRACT_COMPONENT)