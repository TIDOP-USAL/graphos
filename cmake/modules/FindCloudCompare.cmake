unset(CloudCompare_FOUND)

SET(CloudCompare_ROOT "" CACHE PATH "Root folder of CloudCompare dependency")
#SET(CLOUDCOMPARE_VERSION "" CACHE PATH "CloudCompare version")

list(APPEND CloudCompare_CHECK_INCLUDE_DIRS
  /opt/local/include
  /opt/local/include/ufsparse # Mac OS X
  /usr/local/homebrew/include # Mac OS X
  /usr/local/include
  /usr/include
  ${CLOUDCOMPARE_ROOT}/include)
  
list(APPEND CloudCompare_CHECK_LIBRARY_DIRS
  /opt/local/lib
  /opt/local/lib/ufsparse # Mac OS X
  /usr/local/homebrew/lib # Mac OS X
  /usr/local/lib
  /usr/lib
  ${CloudCompare_ROOT}/lib
  ${CloudCompare_ROOT}/lib/cloudcompare)
  
# Additional suffixes to try appending to each search path.
list(APPEND CloudCompare_CHECK_PATH_SUFFIXES
  CloudCompare) # Windows/Ubuntu
  
include(FindPackageHandleStandardArgs)

find_path(CloudCompare_INCLUDE_DIR
  NAMES ccGLWindow.h
  PATHS ${CloudCompare_CHECK_INCLUDE_DIRS}
)

###################################
# CloudCompare_CORE

find_library(CloudCompare_CORE_LIBRARY
  NAMES CC_CORE_LIB CCCoreLib 
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)

find_library(CloudCompare_CORE_LIBRARY_DEBUG
  NAMES CC_CORE_LIBd CCCoreLibd
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)

################################### 


###################################
# CloudCompare_DB

find_library(CloudCompare_DB_LIBRARY
  NAMES QCC_DB_LIB
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)

find_library(CloudCompare_DB_LIBRARY_DEBUG
  NAMES QCC_DB_LIBd
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)

################################### 


###################################  
# CloudCompare_GL

find_library(CloudCompare_GL_LIBRARY
  NAMES QCC_GL_LIB
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)  
 
find_library(CloudCompare_GL_LIBRARY_DEBUG
  NAMES QCC_GL_LIBd
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)   

################################### 


###################################

find_library(CloudCompare_IO_LIBRARY
  NAMES QCC_IO_LIB
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)  

find_library(CloudCompare_IO_LIBRARY_DEBUG
  NAMES QCC_IO_LIBd
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)  

################################### 
 
###################################

find_library(CloudCompare_FBO_LIBRARY
  NAMES CC_FBO_LIB
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)  
find_library(CloudCompare_FBO_LIBRARY_DEBUG
  NAMES CC_FBO_LIBd
  PATHS ${CloudCompare_CHECK_LIBRARY_DIRS}
)  

################################### 

find_package_handle_standard_args(CloudCompare
    FOUND_VAR CloudCompare_FOUND
    REQUIRED_VARS
       #CloudCompare_VERSION
       CloudCompare_INCLUDE_DIR
       CloudCompare_CORE_LIBRARY
       CloudCompare_CORE_LIBRARY_DEBUG
       CloudCompare_DB_LIBRARY
       CloudCompare_DB_LIBRARY_DEBUG
       CloudCompare_GL_LIBRARY
       CloudCompare_GL_LIBRARY_DEBUG
       CloudCompare_IO_LIBRARY
       CloudCompare_IO_LIBRARY_DEBUG
       CloudCompare_FBO_LIBRARY
       CloudCompare_FBO_LIBRARY_DEBUG)
  
if(CloudCompare_FOUND)

    #string(REPLACE "." ";" VERSION_LIST ${CloudCompare_VERSION})
    #list(GET VERSION_LIST 0 CloudCompare_VERSION_MAJOR)
    #list(GET VERSION_LIST 1 CloudCompare_VERSION_MINOR)
    #list(GET VERSION_LIST 2 CloudCompare_VERSION_PATCH)

if(MSVC)
    set(CloudCompare_LIBRARIES
      debug 
        ${CloudCompare_CORE_LIBRARY_DEBUG}
      optimized 
        ${CloudCompare_CORE_LIBRARY}
      debug
        ${CloudCompare_DB_LIBRARY_DEBUG}
      optimized 
        ${CloudCompare_DB_LIBRARY}
      debug
        ${CloudCompare_GL_LIBRARY_DEBUG}
      optimized
        ${CloudCompare_GL_LIBRARY}
      debug
        ${CloudCompare_IO_LIBRARY_DEBUG}
      optimized
        ${CloudCompare_IO_LIBRARY}
      debug
        ${CloudCompare_FBO_LIBRARY_DEBUG}
      optimized
        ${CloudCompare_FBO_LIBRARY})
else()
    set(CloudCompare_LIBRARIES
        ${CloudCompare_CORE_LIBRARY}
        ${CloudCompare_DB_LIBRARY}
        ${CloudCompare_GL_LIBRARY}
        ${CloudCompare_IO_LIBRARY}
        ${CloudCompare_FBO_LIBRARY})
endif()
	
    set(CloudCompare_FOUND TRUE)
    message(STATUS "\n")
    message(STATUS "Found CloudCompare")
    #message(STATUS "  Version: CloudCompare_VERSION")
    message(STATUS "  Include: ${CloudCompare_INCLUDE_DIR}")
    message(STATUS "  Library: ${CloudCompare_CORE_LIBRARY}")
    message(STATUS "  Library: ${CloudCompare_DB_LIBRARY}")
    message(STATUS "  Library: ${CloudCompare_GL_LIBRARY}")
    message(STATUS "  Library: ${CloudCompare_IO_LIBRARY}")
    message(STATUS "  Library: ${CloudCompare_FBO_LIBRARY}")


    add_compile_definitions(CC_VIEWER)
    add_compile_definitions(CC_USE_AS_DLL)
    add_compile_definitions(QCC_DB_USE_AS_DLL)
    add_compile_definitions(QCC_IO_USE_AS_DLL)
    add_compile_definitions(CC_CORE_LIB_USES_FLOAT)
	
else()
    if(CloudCompare_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find CloudCompare")
    endif()
endif()
