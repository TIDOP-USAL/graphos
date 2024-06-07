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

if (GRAPHOS_GUI)

include(TidopUtils)

include_directories(${CMAKE_SOURCE_DIR}/third_party)
include_directories(${CMAKE_SOURCE_DIR}/third_party/EDLfilter)

project(Widgets LANGUAGES CXX)

add_files_to_project(${PROJECT_NAME}
                     SOURCE_FILES
                         SiftWidget.cpp
                         ThumbnailsWidget.cpp
                         LogWidget.cpp
                         StartPageWidget.cpp
                         FeatureMatchingWidget.cpp
                         SmvsWidget.cpp
                         CmvsPmvsWidget.cpp
                         MvsWidget.cpp
                         NvmFormatWidget.cpp
                         BundlerFormatWidget.cpp
                         MveFormatWidget.cpp
                         PointCloudCSVFormatWidget.cpp
                         OriTxtFormatWidget.cpp
                         GraphicItem.cpp
                         GraphicViewer.cpp
                         Viewer3d.cpp
                         ImageMenu.cpp
                         TabWidget.cpp
                         ProgressBarDialog.cpp
                         ProgressBarWidget.cpp
                         settings/FeatViewerSettingsWidget.cpp
                         settings/MatchViewerSettingsWidget.cpp
                         ${CMAKE_SOURCE_DIR}/third_party/EDLfilter/ccEDLFilter.cpp
                     HEADER_FILES
                         GraphosWidget.h
                         SiftWidget.h
                         ThumbnailsWidget.h
                         LogWidget.h
                         StartPageWidget.h
                         FeatureMatchingWidget.h
                         SmvsWidget.h
                         CmvsPmvsWidget.h
                         MvsWidget.h
                         NvmFormatWidget.h
                         BundlerFormatWidget.h
                         MveFormatWidget.h
                         PointCloudCSVFormatWidget.h
                         OriTxtFormatWidget.h
                         GraphicItem.h
                         GraphicViewer.h
                         Viewer3d.h
                         ImageMenu.h
                         TabWidget.h
                         ProgressBarDialog.h
                         ProgressBarWidget.h
                         settings/FeatViewerSettingsWidget.h
                         settings/MatchViewerSettingsWidget.h
                         ${CMAKE_SOURCE_DIR}/third_party/EDLfilter/ccEDLFilter.h)


# EDL Shaders
set(SHADER_EDL ${CMAKE_SOURCE_DIR}/third_party/EDLfilter/shaders/EDL)

file(GLOB shaderFiles ${SHADER_EDL}/*.frag ${SHADER_EDL}/*.vert)
foreach(filename ${shaderFiles} )
    file(COPY ${filename} DESTINATION ${CMAKE_BINARY_DIR}/bin/Release/shaders/EDL)
    file(COPY ${filename} DESTINATION ${CMAKE_BINARY_DIR}/bin/Debug/shaders/EDL)
endforeach()

add_library(${PROJECT_NAME} ${LIB_TYPE}
            ${PROJECT_SOURCE_FILES}
            ${PROJECT_HEADER_FILES})

add_library(Graphos::${PROJECT_NAME} 
            ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${CLOUDCOMPARE_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC
                      TidopLib::Core
                      TidopLib::Img
                      ${CLOUDCOMPARE_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES 
                      FOLDER "lib"
                      OUTPUT_NAME "graphos_widgets")

install(TARGETS ${PROJECT_NAME} 
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/widgets
        FILES_MATCHING PATTERN "*.h")

set(GRAPHOS_LIBRARIES ${GRAPHOS_LIBRARIES} ${PROJECT_NAME})

endif(GRAPHOS_GUI)