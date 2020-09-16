cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)


# Proyecto INSPECTOR (Inspección y mantenimiento en entornos complejos desasistidos)
project(inspector 
        VERSION 0.2.0
        LANGUAGES CXX)
		
		
		
		
## CONFIGURACIÓN GENERAL    

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sufijo para librerias de debug
set(CMAKE_DEBUG_POSTFIX d)

# Tipos de configuración
set(CMAKE_CONFIGURATION_TYPES Release;Debug)

# MACRO PARA DEPURACION
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")


# Configuracion especifica de Visual Studio
if(MSVC)

  # Nivel de warning
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")

  #/EH (Modelo de control de excepciones)
  #/EHsc captura solo las excepciones de C++ e indica al compilador que suponga que las funciones declaradas como extern "C" jamás inicien una excepción de C++.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

  # Las llamadas a los métodos potencialmente no seguros en la biblioteca estandar de C++ dan lugar
  # a una advertencia del compilador (nivel 3) C4996.
  # Para deshabilitar esta advertencia se debe definir la macro _SCL_SECURE_NO_WARNINGS
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE")

endif()


if (UNIX)

#  include(CheckCXXCompilerFlag)
#  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#  if(COMPILER_SUPPORTS_CXX11)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#  else()
#     message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  
endif()

set (CMAKE_CXX_STANDARD 14)

# Directorio con modulos extra de CMAKE
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})


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



## CONFIGURACIÓN COMPONENTES INSPECTOR

# Activación desactivación de los diferentes componentes de INSPECTOR
OPTION (BUILD_INSPECTOR_IMAGE           "INSPECTOR IMAGE"          OFF)
OPTION (BUILD_INSPECTOR_COMMANDS        "INSPECTOR COMMANDS"       ON )
OPTION (BUILD_INSPECTOR_MICROSERVICES   "INSPECTOR MICROSERVICES"  OFF)

# Activación desactivación de otros componentes 
OPTION (BUILD_TEST                 "Include test"          OFF)
#OPTION (BUILD_INSPECTOR_DOC        "Build documentation"   OFF)

# Activa el gestor de mensajes. Si se desactiva no se mostrarán los mensajes del la librería
# por consola ni se escribirán en el log
OPTION (BUILD_MESSAGE_HANDLER "Active message handle" ON)

# Si se activa esta opción se pueden utilizar los métodos marcados como obsoletos (deprecated)
OPTION (BUILD_DEPRECATED_METHODS "Show deprecated methods and class" OFF)

set(BUILD_TL_CORE YES)

#if(BUILD_MESSAGE_HANDLER)
#  set(TL_MESSAGE_HANDLER YES)
#  message(STATUS "[TidopLib] Message manager enabled")
#else()
#  set(TL_MESSAGE_HANDLER NO)
#  message(STATUS "[TidopLib] Message manager disabled")
#endif()

#if(BUILD_DEPRECATED_METHODS)
#  set(TL_ENABLE_DEPRECATED_METHODS YES)
#  message(WARNING "[TidopLib] Enable deprecated methods and class")
#else()
#  set(TL_ENABLE_DEPRECATED_METHODS NO)
#  message(STATUS "[TidopLib] Disable deprecated methods and class")
#endif()


if(BUILD_TEST)
    enable_testing()
endif(BUILD_TEST)


# CONFIGURACIÓN DEPENDENCIAS

#OPTION (WITH_BOOST    "Include Boost support"                   ON)
if(MSVC)
option (WITH_VLD     "Include Visual Leak Detector Support"    OFF)
endif(MSVC)


# TidopLib

find_package(TidopLib REQUIRED)
include_directories(${TidopLib_INCLUDE_DIRS})
 

#  BOOST                                                                           

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
 	
find_package(Boost COMPONENTS filesystem system REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(HAVE_BOOST YES)
else()
    set(HAVE_BOOST NO)
endif()




#  CUDA  

find_package(CUDA)
include_directories(${CUDA_INCLUDE_DIRS})

if (CUDA_FOUND)
  set(HAVE_CUDA YES)
else()
  set(HAVE_CUDA NO)
endif()



#  OPENCV  

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
                 objdetect)
if (HAVE_CUDA)
 find_package(OpenCV REQUIRED
              COMPONENTS
                 cudaimgproc
                 cudawarping)
endif(HAVE_CUDA)

if(NOT OPENCV_FOUND)
    set(HAVE_OPENCV NO)
else()
    set(HAVE_OPENCV YES)
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

#  Colmap 


find_package(COLMAP REQUIRED)

set(HAVE_COLMAP YES)
include_directories(${COLMAP_INCLUDE_DIRS})
link_directories(${COLMAP_LINK_DIRS})
        
if(MSVC)
  # Some fixes for the Glog library.
  add_definitions("-DGLOG_NO_ABBREVIATED_SEVERITIES")
  add_definitions("-DGL_GLEXT_PROTOTYPES")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
  # Enable object level parallel builds in Visual Studio.
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()


find_package(OpenMP)

if(OPENMP_FOUND)
    message(STATUS "Enabling OpenMP support")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(STATUS "Disabling OpenMP support")
endif()

if(OPENMP_ENABLED AND OPENMP_FOUND)
    message(STATUS "Enabling OpenMP support")
    add_definitions("-DOPENMP_ENABLED")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(STATUS "Disabling OpenMP support")
endif()


if(MSVC)
  # Some fixes for the Glog library.
  add_definitions("-DGLOG_NO_ABBREVIATED_SEVERITIES")
  add_definitions("-DGL_GLEXT_PROTOTYPES")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif()

# find_package(Glog REQUIRED)
# include_directories(${Glog_INCLUDE_DIRS})
# find_package(gflags REQUIRED)
# include_directories(${gflags_INCLUDE_DIRS})


#  QT 

find_package(Qt5 REQUIRED 
             COMPONENTS 
               Core 
               OpenGL  
               Widgets 
               Help 
               Sql 
               Concurrent 
               LinguistTools)

if(BUILD_TEST)
  find_package(Qt5 REQUIRED
               COMPONENTS
                 Test)
endif(BUILD_TEST)

if(BUILD_INSPECTOR_IMAGE)
  find_package(Qt5 REQUIRED
               COMPONENTS
                 OpenGLExtensions)
endif()
  
if (Qt5_POSITION_INDEPENDENT_CODE)
  SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(Qt5_FOUND)
    message(STATUS "Found Qt")
    message(STATUS "  Module : ${Qt5Core_DIR}")
    message(STATUS "  Module : ${Qt5OpenGL_DIR}")
    message(STATUS "  Module : ${Qt5Widgets_DIR}")
    message(STATUS "  Module : ${Qt5Help_DIR}")
    message(STATUS "  Module : ${Qt5Sql_DIR}")
    message(STATUS "  Module : ${Qt5Concurrent_DIR}")
    message(STATUS "  Module : ${Qt5LinguistTools_DIR}")
    if(BUILD_TEST)
      message(STATUS "  Module : ${Qt5Test_DIR}")
    endif(BUILD_TEST)
    if(BUILD_INSPECTOR_IMAGE)
      message(STATUS "  Module : ${Qt5OpenGLExtensions_DIR}")
    endif(BUILD_INSPECTOR_IMAGE)


	include_directories(${Qt5Core_INCLUDE_DIRS}
						${Qt5OpenGL_INCLUDE_DIRS}
						${Qt5Widgets_INCLUDE_DIRS}
						${Qt5Help_INCLUDE_DIRS}
						${Qt5Sql_INCLUDE_DIRS}
						${Qt5Concurrent_INCLUDE_DIRS}
						${Qt5LinguistTools_INCLUDE_DIRS})
	if(BUILD_TEST)
		include_directories(${Qt5Test_INCLUDE_DIRS})
	endif(BUILD_TEST)

	if(BUILD_INSPECTOR_IMAGE)
		include_directories(${Qt5OpenGLExtensions_INCLUDE_DIRS})
	endif(BUILD_INSPECTOR_IMAGE)

endif()

if(BUILD_INSPECTOR_IMAGE)

    find_package(CloudCompare)

    if (NOT CLOUDCOMPARE_FOUND)
        set(HAVE_CLOUDCOMPARE NO)
    else()
        set(HAVE_CLOUDCOMPARE YES)
        include_directories(${CLOUDCOMPARE_INCLUDE_DIRS})
    endif()
  
else()
  
    set(HAVE_CLOUDCOMPARE NO)
  
endif(BUILD_INSPECTOR_IMAGE)


# VLD

if(WITH_VLD)

    find_package(VLD)

    if(NOT VLD_FOUND)
        set(HAVE_VLD NO)
    else()
        set(HAVE_VLD YES)
        include_directories(${VLD_INCLUDE_DIRS})
    endif()

endif(WITH_VLD)

if(BUILD_INSPECTOR_MICROSERVICES)

    find_package(cpprestsdk REQUIRED)
	
	find_package(CURL REQUIRED) 
    include_directories(${CURL_INCLUDE_DIRS})

endif (BUILD_INSPECTOR_MICROSERVICES)


set(TL_MESSAGE_HANDLER YES)
message(STATUS "[TidopLib] Message manager enabled")
configure_file("${PROJECT_SOURCE_DIR}/cmake/config_tl.h.in" "${PROJECT_BINARY_DIR}/config_tl.h")
include_directories("${PROJECT_BINARY_DIR}")
add_subdirectory(third_party)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


#cabeceras con la configuración

configure_file("${PROJECT_SOURCE_DIR}/cmake/config_inspector.h.in" "${PROJECT_BINARY_DIR}/config_inspector.h")

# include del directorio principal
include_directories(${CMAKE_BUILD_DIR})


#Recursos
set(INSPECTOR_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res/res.qrc)


#directorio con el código fuente de los diferentes modulos
add_subdirectory(src)



#directorio con la documentación
#add_subdirectory(doc)

#Se incluye el directorio con los test
if(BUILD_TEST)
    add_subdirectory(test)
endif(BUILD_TEST)





# Install

install(FILES ${PROJECT_BINARY_DIR}/config_tl.h DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/config_inspector.h DESTINATION include)
