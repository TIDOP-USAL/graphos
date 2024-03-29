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

if(BUILD_CREATE_PROJECT_COMPONENT)

    project(CreateProject LANGUAGES CXX)
    
    add_files_to_project(${PROJECT_NAME}
                         SOURCE_FILES
                             CreateProjectComponent.cpp
                             impl/CreateProjectModel.cpp
                             impl/CreateProjectView.cpp
                             impl/CreateProjectPresenter.cpp
                             impl/CreateProjectCommand.cpp
                         HEADER_FILES
                             CreateProjectComponent.h
                             CreateProjectModel.h
                             CreateProjectView.h
                             CreateProjectPresenter.h
                             impl/CreateProjectModel.h
                             impl/CreateProjectView.h
                             impl/CreateProjectPresenter.h
                             impl/CreateProjectCommand.h)
            
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
            DESTINATION include/graphos/components/createproject
            FILES_MATCHING PATTERN "*.h"
            PATTERN "impl" EXCLUDE
            PATTERN "test" EXCLUDE)
        
    set(GRAPHOS_HAVE_CREATE_PROJECT YES CACHE INTERNAL "")
    
    message(STATUS "  Graphos::CreateProject enabled")

    if(BUILD_TEST)
        add_subdirectory(test)
    endif(BUILD_TEST)
	
else()

    message(STATUS "  Graphos::CreateProject disabled")
    
endif(BUILD_CREATE_PROJECT_COMPONENT)