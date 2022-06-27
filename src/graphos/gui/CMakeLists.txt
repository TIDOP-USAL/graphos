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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

#--------------------------------------------------------------------------------------------#
# Configuración para Qt                                                                      #
#--------------------------------------------------------------------------------------------#

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

#--------------------------------------------------------------------------------------------#
# Ejecutable graphos                                                                         #
#--------------------------------------------------------------------------------------------#

#TODO: Subir un nivel y cargar los componentes automaticamente de la carpeta components

project(graphos LANGUAGES CXX)

set(GRAPHOS_SOURCES
    main.cpp
    ComponentsManager.cpp
    MainWindowModel.cpp
    MainWindowPresenter.cpp
    MainWindowView.cpp
    HelpDialog.cpp
    utils/ProgressDialog.cpp
    ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.cpp
)

set(GRAPHOS_HEADERS
    ComponentsManager.h
    MainWindowModel.h
    MainWindowPresenter.h
    MainWindowView.h
    HelpDialog.h
    utils/ProgressDialog.h
    ${CMAKE_SOURCE_DIR}/third_party/easyexif/exif.h
)

if(MSVC)
    list(APPEND GRAPHOS_SOURCES resource.h graphos.rc)
endif(MSVC)

# forms
set(GRAPHOS_FORMS
  MainWindowView.ui
  utils/ProgressDialog.ui
)

# Translation files
### TODO: Por si se quiere añadir soporte para varios idiomas
SET(GRAPHOS_TS_FILES
    ${CMAKE_SOURCE_DIR}/resources/lang/graphos_en.ts
    ${CMAKE_SOURCE_DIR}/resources/lang/graphos_es.ts
)

qt5_add_translation(GRAPHOS_QM_FILES ${GRAPHOS_TS_FILES})

QT5_ADD_RESOURCES(UI_GRAPHOS_RES 
                  ${GRAPHOS_RESOURCES} 
                  ${CMAKE_SOURCE_DIR}/res/icones.qrc)

add_executable(${PROJECT_NAME}
#add_executable(${PROJECT_NAME} WIN32
               ${GRAPHOS_SOURCES}
               ${GRAPHOS_HEADERS}
               ${GRAPHOS_FORMS}
               ${UI_GRAPHOS_RES}
               #${GRAPHOS_TS_FILES}
)

source_group("Form Files"  FILES ${GRAPHOS_FORMS})
source_group("Resource Files"  FILES ${GRAPHOS_RESOURCES} ${UI_GRAPHOS_RES})
#source_group("Translation Files"  FILES ${GRAPHOS_TS_FILES})

foreach(_source IN ITEMS ${GRAPHOS_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Source Files\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${GRAPHOS_HEADERS})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Header Files\\${_group_path}" FILES "${_source}")
endforeach()

target_link_libraries(${PROJECT_NAME}
                      TidopLib::tl_core
                      TidopLib::tl_geom
                      TidopLib::tl_geospatial
                      TidopLib::tl_img
                      TidopLib::tl_math
                      TidopLib::tl_vect
                      TidopLib::tl_graphic
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Help
                      Qt5::Concurrent
                      Qt5::Sql
                      Qt5::PrintSupport
                      Qt5::OpenGL
                      Qt5::OpenGLExtensions
                      $<$<BOOL:${GRAPHOS_HAVE_ABOUT}>:Graphos::Core>
                      $<$<BOOL:${GRAPHOS_HAVE_ABOUT}>:Graphos::Widgets>
                      $<$<BOOL:${GRAPHOS_HAVE_ABOUT}>:Graphos::About>
                      $<$<BOOL:${GRAPHOS_HAVE_CAMERAS}>:Graphos::Cameras>
                      $<$<BOOL:${GRAPHOS_HAVE_CREATE_PROJECT}>:Graphos::CreateProject>
                      $<$<BOOL:${GRAPHOS_HAVE_DENSE}>:Graphos::Dense>
                      $<$<BOOL:${GRAPHOS_HAVE_DTM}>:Graphos::Dtm>
                      $<$<BOOL:${GRAPHOS_HAVE_FEATEXTRACT}>:Graphos::FeatExtract>
                      $<$<BOOL:${GRAPHOS_HAVE_FEATMATCH}>:Graphos::FeatMatch>
                      $<$<BOOL:${GRAPHOS_HAVE_FEATVIEWER}>:Graphos::FeatViewer>
                      $<$<BOOL:${GRAPHOS_HAVE_GEOREFERENCE}>:Graphos::Georeference>
                      $<$<BOOL:${GRAPHOS_HAVE_IMAGE_LOAD}>:Graphos::ImageLoad>
                      $<$<BOOL:${GRAPHOS_HAVE_IMPORT_CAMERAS}>:Graphos::ImportCameras>
                      $<$<BOOL:${GRAPHOS_HAVE_MATCH_VIEWER}>:Graphos::MatchViewer>
                      $<$<BOOL:${GRAPHOS_HAVE_MESH}>:Graphos::Mesh>
                      $<$<BOOL:${GRAPHOS_HAVE_OPEN_PROJECT}>:Graphos::OpenProject>
                      $<$<BOOL:${GRAPHOS_HAVE_ORIENTATION}>:Graphos::Orientation>
                      $<$<BOOL:${GRAPHOS_HAVE_UNDISTORT}>:Graphos::Undistort>
                      $<$<BOOL:${GRAPHOS_HAVE_ORTHOPHOTO}>:Graphos::Orthophoto>
                      $<$<BOOL:${GRAPHOS_HAVE_SETTINGS}>:Settings>
                      CSF
                      ${OpenCV_LIBS}
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::glew
                      ${CERES_LIBRARIES}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      ${GDAL_LIBRARY} 
                      ${PROJ4_LIBRARY}
                      ${Boost_FILESYSTEM_LIBRARY}
                      ${Boost_SYSTEM_LIBRARY}
                      ${Boost_THREAD_LIBRARY}
                      ${Boost_DATE_TIME_LIBRARY}
                      ${Boost_CHRONO_LIBRARY}
                      ${Boost_PROGRAM_OPTIONS_LIBRARY}
                      ${Boost_REGEX_LIBRARY}
                      ${CLOUDCOMPARE_LIBRARIES}
                      $<$<BOOL:${HAVE_VLD}>:${VLD_LIBRARY}>)


if(ENABLE_SOLUTION_FOLDERS)
  set_target_properties(${PROJECT_NAME}  PROPERTIES FOLDER "graphos")
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

##############################################################
# Install                                                    #
##############################################################

if(WIN32)
    if (MSVC AND NOT BUILD_SHARED_LIBS)
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:atlthunk.lib /NODEFAULTLIB:atlsd.lib /DEBUG")
    endif()

    install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION bin
            COMPONENT bin)
endif()
