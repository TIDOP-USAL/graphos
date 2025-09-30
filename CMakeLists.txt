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

cmake_minimum_required(VERSION 3.10.0...3.22)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

if(POLICY CMP0146) 
    cmake_policy(SET CMP0146 OLD) 
endif()

project(GRAPHOS 
        VERSION 2.0.0
        DESCRIPTION "GRAPHOS (InteGRAted PHOtogrammetric Suite)"
        HOMEPAGE_URL "http://tidop.usal.es"
        LANGUAGES CXX)


message(STATUS "\n${PROJECT_NAME} ${GRAPHOS_VERSION}")
message(STATUS ${GRAPHOS_DESCRIPTION})
message(STATUS ${GRAPHOS_HOMEPAGE_URL})


# Configuration 

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
set(CMAKE_CONFIGURATION_TYPES Release;Debug)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


#if (UNIX)
#
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#  
#endif()

# Compile options 

#add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
#add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(MSVC)
    add_compile_options("/W3" "/EHsc" "-D_SCL_SECURE_NO_WARNINGS" "-D_CRT_SECURE_NO_DEPRECATE")
else()
    #add_compile_options("-Wall" "-Wextra" "-Werror")
    add_compile_options("-Wall" "-Wextra")
endif()


if(MSVC)

    if(MSVC_VERSION EQUAL 1900)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14")
    elseif(MSVC_VERSION MATCHES "^191[0-9]$")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
    elseif(MSVC_VERSION MATCHES "^192[0-9]$")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
    elseif(MSVC_VERSION MATCHES "^193[0-9]$")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
    endif()

endif()


# Directorio con modulos extra de CMAKE
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
if(MSVC)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/msvc" ${CMAKE_MODULE_PATH})
endif()

set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

add_definitions(-DUNICODE)

# Se permite elegir entre libreria estatica o dinamica
OPTION (BUILD_SHARED_LIBS "Build Shared Libraries" OFF)

# Por defecto se establece el tipo de librería estática
set (LIB_TYPE STATIC)

if (BUILD_SHARED_LIBS)
  # Se configura como libreria estática
  set (LIB_TYPE SHARED)
  
  if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")
  endif(MSVC)
  
endif (BUILD_SHARED_LIBS)



include(CompilerInfo)
include(OperatingSystemInfo)



include(CMakeDependentOption)

##########################################################################
#  GRAPHOS Components                                                    #
#------------------------------------------------------------------------#
#                                                                        #
# Activación/desactivación de los diferentes componentes de Graphos      #
# Configuración de la librería                                           #
#                                                                        #
##########################################################################


unset(GRAPHOS_HAVE_ABOUT CACHE)
unset(GRAPHOS_HAVE_CAMERAS CACHE)
unset(GRAPHOS_HAVE_CREATE_PROJECT CACHE)
unset(GRAPHOS_HAVE_DENSE CACHE)
unset(GRAPHOS_HAVE_DEM CACHE)
unset(GRAPHOS_HAVE_FEATEXTRACT CACHE)
unset(GRAPHOS_HAVE_FEATMATCH CACHE)
unset(GRAPHOS_HAVE_FEATVIEWER CACHE)
unset(GRAPHOS_HAVE_GCPS CACHE)
unset(GRAPHOS_HAVE_SCALE CACHE)
unset(GRAPHOS_HAVE_FLOOR_LEVEL CACHE)
unset(GRAPHOS_HAVE_IMAGE_LOAD CACHE)
unset(GRAPHOS_HAVE_IMPORT_CAMERAS CACHE)
unset(GRAPHOS_HAVE_EXPORT_CAMERA_POSES CACHE)
unset(GRAPHOS_HAVE_EXPORT_POINT_CLOUD CACHE)
unset(GRAPHOS_HAVE_EXPORT_MESH CACHE)
unset(GRAPHOS_HAVE_MATCH_VIEWER CACHE)
unset(GRAPHOS_HAVE_MESH CACHE)
unset(GRAPHOS_HAVE_OPEN_PROJECT CACHE)
unset(GRAPHOS_HAVE_RECENT_PROJECTS CACHE)
unset(GRAPHOS_HAVE_SAVE_PROJECT CACHE)
unset(GRAPHOS_HAVE_SAVE_PROJECT_AS CACHE)
unset(GRAPHOS_HAVE_CLOSE_PROJECT CACHE)
unset(GRAPHOS_HAVE_ORIENTATION CACHE)
unset(GRAPHOS_HAVE_ORTHOPHOTO CACHE)
unset(GRAPHOS_HAVE_SETTINGS CACHE)
unset(GRAPHOS_HAVE_UNDISTORT CACHE)
unset(GRAPHOS_HAVE_PROPERTIES CACHE)
unset(GRAPHOS_HAVE_VIDEO_LOAD CACHE)
unset(GRAPHOS_HAVE_CRS CACHE)
unset(GRAPHOS_GUI CACHE)

option(BUILD_ABOUT_COMPONENT          "Build About Component"         ON)
option(BUILD_CAMERAS_COMPONENT        "Build Cameras Component"       ON)
option(BUILD_CREATE_PROJECT_COMPONENT "Build Create Project Component"  ON)
option(BUILD_DENSE_COMPONENT "Build Dense Component"  ON)
option(BUILD_DEM_COMPONENT "Build DEM Component"  ON)
option(BUILD_FEATEXTRACT_COMPONENT "Build Feature Extractor Component"  ON)
option(BUILD_FEATMATCH_COMPONENT "Build Feature Match Component"  ON)
option(BUILD_FEATVIEWER_COMPONENT "Build Feature Viewer Component"  ON)
option(BUILD_GCPS_COMPONENT "Build Ground Control Points Component"  ON)
option(BUILD_SCALE_COMPONENT "Build Scale Component"  ON)
option(BUILD_FLOOR_LEVEL_COMPONENT "Build Floor Level Component"  ON)
option(BUILD_IMAGE_LOAD_COMPONENT "Build Image Load Component"  ON)
option(BUILD_IMPORT_CAMERAS_COMPONENT "Build Import Cameras Component"  ON)
option(BUILD_EXPORT_CAMERA_POSES_COMPONENT "Build Export Camera Poses"  ON)
option(BUILD_EXPORT_POINT_CLOUD_COMPONENT "Build Point Cloud"  ON)
option(BUILD_EXPORT_MESH_COMPONENT "Build Export Mesh"  ON)
option(BUILD_MATCH_VIEWER_COMPONENT "Build Match Viewer Component"  ON)
option(BUILD_MESH_COMPONENT "Build Mesh Component"  ON)
option(BUILD_OPEN_PROJECT_COMPONENT "Build Open Project Component"  ON)
option(BUILD_RECENT_PROJECTS_COMPONENT "Build Recent Projects Component"  ON)
option(BUILD_SAVE_PROJECT_COMPONENT "Build Save Project Component"  ON)
option(BUILD_SAVE_PROJECT_AS_COMPONENT "Build Save Project As... Component"  ON)
option(BUILD_CLOSE_PROJECT_COMPONENT "Build Close Project Component"  ON)
option(BUILD_ORIENTATION_COMPONENT "Build Orientation Component"  ON)
option(BUILD_ORTHOPHOTO_COMPONENT "Build Orthophoto Component"  ON)
option(BUILD_SETTINGS_COMPONENT "Build Settings Component"  ON)
option(BUILD_UNDISTORT_COMPONENT "Build Undistort Component"  ON)
option(BUILD_PROPERTIES_COMPONENT "Build Properties Component"  ON)
option(BUILD_VIDEO_LOAD_COMPONENT "Build Video load Component"  ON)
option(BUILD_CRS_COMPONENT "Build Coordinate Reference System Component"  ON)

# Activación desactivación de otros componentes 
option (BUILD_TEST                 "Include test"          OFF)
option (BUILD_PLUGINS              "Build Plugins"         ON )
option (BUILD_GUI                  "Build GUI"             ON )
option (BUILD_TRANSLATION          "Build translations"    OFF)

# Si se activa esta opción se pueden utilizar los métodos marcados como obsoletos (deprecated)
option (BUILD_DEPRECATED_METHODS "Show deprecated methods and class" OFF)

if(MSVC)
option (WITH_VLD     "Include Visual Leak Detector Support"    OFF)
endif(MSVC)
#option(WITH_PIXKIT   "Include PixKit"                  OFF)
option (WITH_CUDA     "Include Cuda Support"    ON)


message(STATUS "\nGRAPHOS configuration:\n")

if(BUILD_GUI)
    message(STATUS "  [GRAPHOS] Build GUI")
    set(GRAPHOS_GUI YES)
else()
    message(STATUS "  [GRAPHOS] GUI disabled")
	set(GRAPHOS_GUI NO)
endif()

if(BUILD_TRANSLATION)
    message(STATUS "  [GRAPHOS] Build translation")
    set(GRAPHOS_TRANSLATION YES)
else()
    message(STATUS "  [GRAPHOS] Translation disabled")
	set(GRAPHOS_TRANSLATION NO)
endif()




##########################################################################
#  Dependencies                                                          #
#------------------------------------------------------------------------#
# - TidopLib                  REQUIRED                                   #
# - BOOST                     REQUIRED                                   #
# - CUDA                      REQUIRED                                   #
# - OpenCV                    REQUIRED                                   #
# - COLMAP                    REQUIRED                                   #
# - OpenMP                    OPTIONAL                                   #
# - Qt5                       REQUIRED                                   #
# - CloudCompare              REQUIRED                                   #
# - VLD                       OPTIONAL                                   #
##########################################################################

message(STATUS "\nDependency loading:\n")



##########################################################################
# TidopLib (REQUIRED)                                                    #
##########################################################################      

set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
find_package(TidopLib 3.3 REQUIRED)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

if(TidopLib_FOUND)
    message(STATUS "  Found TidopLib (version ${TidopLib_VERSION})")
else()
    message(STATUS "Not Found TidopLib")
endif()

find_package(COPCLIB REQUIRED)

##########################################################################
# BOOST (REQUIRED)                                                       #
##########################################################################                                                                        

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
 	
set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")	
find_package(Boost COMPONENTS filesystem system unit_test_framework REQUIRED)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

if(Boost_FOUND)
    message(STATUS "  Found Boost (version ${Boost_VERSION})")
    include_directories(${Boost_INCLUDE_DIRS})
    set(HAVE_BOOST YES)
else()
    set(HAVE_BOOST NO)
endif()


##########################################################################
# OpenCV  (REQUIRED)                                                     #
##########################################################################

#  CUDA  

if (WITH_CUDA)

set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
    find_package(CUDA)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
  
    if (CUDA_FOUND)
        set(HAVE_CUDA YES)
        include_directories(${CUDA_INCLUDE_DIRS})
        message(STATUS "  Found Cuda (version ${CUDA_VERSION})")
    else()
        set(HAVE_CUDA NO)
    endif()

endif()

set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
find_package(OpenCV REQUIRED
             COMPONENTS
                 core
                 highgui
                 photo
                 xfeatures2d
                 imgproc
                 imgcodecs
                 calib3d
                 features2d
                 flann
                 objdetect
                 stitching)
	
if (HAVE_CUDA)
 find_package(OpenCV REQUIRED
              COMPONENTS
                 cudaimgproc
                 cudawarping)
endif(HAVE_CUDA)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

if(NOT OPENCV_FOUND)
    set(HAVE_OPENCV NO)
else()
    message(STATUS "  Found OpenCV (version ${OpenCV_VERSION})")
    set(HAVE_OPENCV YES)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()



##########################################################################
# Colmap (REQUIRED)                                                      #
##########################################################################   

set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
find_package(COLMAP REQUIRED)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

if(NOT COLMAP_FOUND)
    set(HAVE_COLMAP NO)
else()
    set(HAVE_COLMAP YES)
    include_directories(${COLMAP_INCLUDE_DIRS})
    link_directories(${COLMAP_LINK_DIRS})
    message(STATUS "  Found COLMAP")        
endif()

if(MSVC)
  # Some fixes for the Glog library.
  add_definitions("-DGLOG_NO_ABBREVIATED_SEVERITIES")
  add_definitions("-DGL_GLEXT_PROTOTYPES")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  # Enable object level parallel builds in Visual Studio.
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()


##########################################################################
# GLEW (REQUIRED)                                                        #
##########################################################################   

find_package(GLEW REQUIRED)


##########################################################################
# QT (REQUIRED)                                                          #
##########################################################################

set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")

find_package(Qt5 REQUIRED 
             COMPONENTS 
               Core 
               OpenGL  
               Sql 
               Concurrent 
               LinguistTools
               PrintSupport)
			   
if(BUILD_GUI)
find_package(Qt5 REQUIRED 
             COMPONENTS 
			   Widgets
               Network
               Help)		   
endif(BUILD_GUI)

if(BUILD_TEST)
  find_package(Qt5 REQUIRED
               COMPONENTS
                 Test)
endif(BUILD_TEST)

find_package(Qt5 REQUIRED
             COMPONENTS
               OpenGLExtensions)
			   
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
	
if (Qt5_POSITION_INDEPENDENT_CODE)
  SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(Qt5_FOUND)
    message(STATUS "  Found Qt (version ${Qt5_VERSION})")

	include_directories(${Qt5Core_INCLUDE_DIRS}
                        ${Qt5OpenGL_INCLUDE_DIRS}
                        ${Qt5Widgets_INCLUDE_DIRS}
                        ${Qt5Help_INCLUDE_DIRS}
                        ${Qt5Sql_INCLUDE_DIRS}
                        ${Qt5Concurrent_INCLUDE_DIRS}
                        ${Qt5LinguistTools_INCLUDE_DIRS}
                        ${Qt5OpenGLExtensions_INCLUDE_DIRS})
	if(BUILD_TEST)
		include_directories(${Qt5Test_INCLUDE_DIRS})
	endif(BUILD_TEST)

endif()


if(BUILD_GUI)
set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
find_package(CloudCompare REQUIRED)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

if (NOT CloudCompare_FOUND)
    set(HAVE_CloudCompare NO)
else()
    set(HAVE_CloudCompare YES)
    include_directories(${CloudCompare_INCLUDE_DIR})
	message(STATUS "  Found CloudCompare")
endif()
endif(BUILD_GUI)


# VLD
##########################################################################
# VLD (OPTIONAL)                                                         #
##########################################################################

if(WITH_VLD)

    set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
    find_package(VLD)
    set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
	
    if(NOT VLD_FOUND)
        set(HAVE_VLD NO)
    else()
        set(HAVE_VLD YES)
        include_directories(${VLD_INCLUDE_DIRS})
	    message(STATUS "  Found VLD")
    endif()

endif(WITH_VLD)

#[[
if(WITH_PIXKIT)

    find_package(PIXKIT REQUIRED)
    if(NOT PIXKIT_FOUND)
        set(HAVE_PIXKIT NO)
    else()
        set(HAVE_PIXKIT YES)
        include_directories(${PIXKIT_INCLUDE_DIRS})
    endif()

endif(WITH_PIXKIT)
]]

if (BUILD_DEM_COMPONENT)

    set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
    find_package(CGAL REQUIRED)
	set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
		
    message(STATUS "  Found CGAL (version ${CGAL_VERSION})")
    #find_package(Boost OPTIONAL_COMPONENTS serialization iostreams)
    #
    #include(CGAL_Eigen3_support)
    #include(CGAL_Boost_serialization_support)
    #include(CGAL_Boost_iostreams_support)

endif ()


##########################################################################
# QGeoView (REQUIRED)                                                    #
##########################################################################

set(CMAKE_MESSAGE_LOG_LEVEL "ERROR")
find_package(QGeoView REQUIRED)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
    
if(NOT QGeoView_FOUND)
    set(HAVE_QGEOVIEW NO)
else()
    set(HAVE_QGEOVIEW YES)
    include_directories(${QGeoView_INCLUDE_DIRS})
    message(STATUS "  Found QGeoView")
endif()



include_directories("${PROJECT_BINARY_DIR}")
add_subdirectory(third_party)


message(STATUS "\nGRAPHOS modules:\n")

# include del directorio principal
include_directories(${CMAKE_BUILD_DIR})


#Recursos
set(GRAPHOS_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc)


#directorio con el código fuente de los diferentes modulos
add_subdirectory(src)


#directorio con la documentación
#add_subdirectory(doc)

message("\n")


#cabecera con la configuración

configure_file("${PROJECT_SOURCE_DIR}/cmake/config_graphos.h.in" "${PROJECT_BINARY_DIR}/config_graphos.h")




##########################################################################
# Plugins                                                                #
##########################################################################

if (BUILD_PLUGINS)

    message(STATUS "\nGRAPHOS plugins:\n")

    add_subdirectory(plugins)
    
    message("\n")

endif(BUILD_PLUGINS)





##########################################################################
# Test                                                                   #
##########################################################################

if(BUILD_TEST)
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TEST)




##########################################################################
# Install                                                                #
##########################################################################

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)


# Graphos configuration header
install(FILES ${PROJECT_BINARY_DIR}/config_graphos.h DESTINATION include)

# Graphos camera database
install(FILES 
            ${CMAKE_CURRENT_SOURCE_DIR}/res/cameras.db
            ${CMAKE_CURRENT_SOURCE_DIR}/res/cameras_user.db
        DESTINATION
            bin)

set(GRAPHOS_CONFIG_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}")
set(GRAPHOS_CONFIG_VERSION_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}/GraphosConfigVersion.cmake")
set(GRAPHOS_CONFIG_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}/GraphosConfig.cmake")
set(GRAPHOS_NAMESPACE "Graphos::")
set(GRAPHOS_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")


# Write package version file
write_basic_package_version_file(
  ${GRAPHOS_CONFIG_VERSION_CMAKE_PATH}
  VERSION ${GRAPHOS_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "cmake/GraphosConfig.cmake.in"
    ${GRAPHOS_CONFIG_CMAKE_PATH}
    INSTALL_DESTINATION "${GRAPHOS_CONFIG_CMAKE_INSTALL_DIR}"
)

install(
    FILES "${GRAPHOS_CONFIG_CMAKE_PATH}" "${GRAPHOS_CONFIG_VERSION_CMAKE_PATH}"
    DESTINATION "${GRAPHOS_CONFIG_CMAKE_INSTALL_DIR}")

install(
    EXPORT "${TARGETS_EXPORT_NAME}"
    NAMESPACE "${GRAPHOS_NAMESPACE}"
    DESTINATION "${GRAPHOS_CONFIG_CMAKE_INSTALL_DIR}")

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
        DESTINATION "${GRAPHOS_CONFIG_CMAKE_INSTALL_DIR}"
        FILES_MATCHING PATTERN "*.cmake")
