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


if(BUILD_VIDEO_LOAD)

    project(video_load)

    set(VIDEO_LOAD_SOURCES
        impl/LoadFromVideoComponent.cpp
        impl/LoadFromVideoModel.cpp
        impl/LoadFromVideoPresenter.cpp
        impl/LoadFromVideoView.cpp)
                
    set(VIDEO_LOAD_HEADERS
        LoadFromVideoModel.h
        LoadFromVideoPresenter.h
        LoadFromVideoView.h
        impl/LoadFromVideoComponent.h
        impl/LoadFromVideoModel.h
        impl/LoadFromVideoPresenter.h
        impl/LoadFromVideoView.h)
                
    add_library(${PROJECT_NAME} ${LIB_TYPE}
                ${VIDEO_LOAD_SOURCES}
                ${VIDEO_LOAD_HEADERS})
    
    foreach(_source IN ITEMS ${VIDEO_LOAD_SOURCES})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "/" "\\" _group_path "${_source_path}")
        source_group("Source Files\\${_group_path}" FILES "${_source}")
    endforeach()
    
    foreach(_source IN ITEMS ${VIDEO_LOAD_HEADERS})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "/" "\\" _group_path "${_source_path}")
        source_group("Header Files\\${_group_path}" FILES "${_source}")
    endforeach()
    
    
    #target_include_directories(${PROJECT_NAME} PUBLIC
    #                           $<BUILD_INTERFACE:${TidopLib_SOURCE_DIR}/src>
    #                           $<INSTALL_INTERFACE:include>)
    #target_include_directories(${PROJECT_NAME} PRIVATE ${GDAL_INCLUDE_DIR})
    #
    #target_link_libraries(${PROJECT_NAME} PUBLIC
    #                      ${Boost_FILESYSTEM_LIBRARY} 
    #                      ${Boost_SYSTEM_LIBRARY}
    #                      $<$<BOOL:${TL_HAVE_OPENMP}>:OpenMP::OpenMP_CXX>
    #                      $<$<BOOL:${TL_HAVE_GDAL}>:${GDAL_LIBRARY}>)
    
    set_property(TARGET ${PROJECT_NAME}
                 PROPERTY FOLDER "modules")
    
        install(TARGETS ${PROJECT_NAME}
                EXPORT "${TARGETS_EXPORT_NAME}"
                LIBRARY DESTINATION ${TL_INSTALL_BINARIES_PREFIX}/lib 
                ARCHIVE DESTINATION ${TL_INSTALL_BINARIES_PREFIX}/lib)
    
    
    
    install(DIRECTORY .
            DESTINATION include/graphos/components/video_load
            FILES_MATCHING PATTERN "*.h")
    
    
    set(Graphos_LIBRARIES ${Graphos_LIBRARIES} ${PROJECT_NAME})
    
    set(TL_HAVE_CORE YES CACHE INTERNAL "")
    message(STATUS "  Graphos::video_load enabled")
    
else()

    message(STATUS "  Graphos::video_load disabled")
    #message(STATUS "  TidopLib::tl_math disabled         [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_geom disabled         [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_img disabled          [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_imgprocess disabled   [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_graphic disabled      [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_featmatch disabled    [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_vect disabled         [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_geospatial disabled   [Module dependent on tl_core that is deactivated]")
    #message(STATUS "  TidopLib::tl_experimental disabled [Module dependent on tl_core that is deactivated]")
  
endif(BUILD_VIDEO_LOAD)