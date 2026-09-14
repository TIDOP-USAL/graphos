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


project(Framework LANGUAGES CXX)

add_files_to_project(${PROJECT_NAME}
                     SOURCE_FILES
                         app/Application.cpp
                         component/ComponentBase.cpp
                         component/MultiComponentBase.cpp
                         component/TaskComponent.cpp
                         task/ProgressHandler.cpp
                         task/TaskPresenter.cpp         
                         task/TaskView.cpp
                     HEADER_FILES
                         app/Application.h
                         command/Command.h
                         component/Component.h
                         component/ComponentBase.h
                         component/MultiComponent.h
                         component/MultiComponentBase.h
                         component/TaskComponent.h
                         mvp/DialogView.h
                         mvp/FileDialogView.h
                         mvp/MenuView.h
                         mvp/Model.h
                         mvp/Presenter.h
                         mvp/View.h
                         task/ProgressHandler.h
                         task/TaskPresenter.h            
                         task/TaskView.h)

	
add_library(${PROJECT_NAME} ${GRAPHOS_LIB_TYPE}
            ${PROJECT_SOURCE_FILES}
            ${PROJECT_HEADER_FILES})
            
add_library(Graphos::${PROJECT_NAME} 
            ALIAS ${PROJECT_NAME})


target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${GRAPHOS_SOURCE_DIR}/src>
                           $<INSTALL_INTERFACE:include>)

target_include_directories(${PROJECT_NAME} PRIVATE 
                           ${GDAL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
                      Graphos::Core
                      Qt6::Core
                      $<$<BOOL:${GRAPHOS_GUI}>:Qt6::Widgets>
                      ${GDAL_LIBRARY} 
                      PROJ::proj)


set_target_properties(${PROJECT_NAME} PROPERTIES 
                      FOLDER "lib"
                      OUTPUT_NAME "graphos_framework")

install(TARGETS ${PROJECT_NAME}
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/core
        FILES_MATCHING PATTERN "*.h")

set(GRAPHOS_LIBRARIES ${GRAPHOS_LIBRARIES} ${PROJECT_NAME})
