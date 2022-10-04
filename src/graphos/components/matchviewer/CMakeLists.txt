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


if(BUILD_MATCH_VIEWER_COMPONENT)

    project(MatchViewer LANGUAGES CXX)
    
    
    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    # Instruct CMake to run moc automatically when needed
    set(CMAKE_AUTOMOC ON)
    # Create code from a list of Qt designer ui files
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)
    
    
    add_files_to_project(${PROJECT_NAME}
                         SOURCE_FILES
                             MatchViewerComponent.cpp
                             impl/MatchViewerModel.cpp
                             impl/MatchViewerView.cpp
                             impl/MatchViewerPresenter.cpp
                         HEADER_FILES
                             MatchViewerComponent.h
                             MatchViewerModel.h
                             MatchViewerView.h
                             MatchViewerPresenter.h
                             impl/MatchViewerModel.h
                             impl/MatchViewerView.h
                             impl/MatchViewerPresenter.h)
            
    add_library(${PROJECT_NAME} ${LIB_TYPE}
                ${PROJECT_SOURCE_FILES}
                ${PROJECT_HEADER_FILES})
                
    add_library(Graphos::${PROJECT_NAME} 
                ALIAS ${PROJECT_NAME})
    
    target_link_libraries(${PROJECT_NAME} PRIVATE 
                          Qt5::Core
                          Qt5::Widgets
                          Graphos::Core
                          Eigen3::Eigen
                          glog::glog)
    
    set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Components")
    
    install(TARGETS ${PROJECT_NAME}
            EXPORT "${TARGETS_EXPORT_NAME}"
            LIBRARY DESTINATION lib 
            ARCHIVE DESTINATION lib)
                        
    install(DIRECTORY .
            DESTINATION include/graphos/components/matchviewer
            FILES_MATCHING PATTERN "*.h"
            PATTERN "impl" EXCLUDE)
        

    set(GRAPHOS_HAVE_MATCH_VIEWER YES CACHE INTERNAL "")
    
    message(STATUS "  Graphos::MatchViewer enabled")
    
else()

    message(STATUS "  Graphos::MatchViewer disabled")
    
endif(BUILD_MATCH_VIEWER_COMPONENT)