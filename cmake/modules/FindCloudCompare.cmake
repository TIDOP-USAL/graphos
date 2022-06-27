unset(CLOUDCOMPARE_FOUND)

SET(CLOUDCOMPARE_ROOT "" CACHE PATH "Root folder of CloudCompare dependency")

list(APPEND CLOUDCOMPARE_CHECK_INCLUDE_DIRS
  /opt/local/include
  /opt/local/include/ufsparse # Mac OS X
  /usr/local/homebrew/include # Mac OS X
  /usr/local/include
  /usr/include
  ${CLOUDCOMPARE_ROOT}/include)
  
list(APPEND CLOUDCOMPARE_CHECK_LIBRARY_DIRS
  /opt/local/lib
  /opt/local/lib/ufsparse # Mac OS X
  /usr/local/homebrew/lib # Mac OS X
  /usr/local/lib
  /usr/lib
  ${CLOUDCOMPARE_ROOT}/lib)
  
# Additional suffixes to try appending to each search path.
list(APPEND CLOUDCOMPARE_CHECK_PATH_SUFFIXES
  CloudCompare) # Windows/Ubuntu
  
include(FindPackageHandleStandardArgs)

find_path(CLOUDCOMPARE_INCLUDE_DIR
  NAMES ccGLWindow.h
  PATHS ${CLOUDCOMPARE_CHECK_INCLUDE_DIRS}
)

###################################
# CLOUDCOMPARE_CORE

find_library(CLOUDCOMPARE_CORE_LIBRARY
  NAMES CC_CORE_LIB
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)

find_library(CLOUDCOMPARE_CORE_LIBRARY_DEBUG
  NAMES CC_CORE_LIBd
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)

################################### 


###################################
# CLOUDCOMPARE_DB

find_library(CLOUDCOMPARE_DB_LIBRARY
  NAMES QCC_DB_LIB
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)

find_library(CLOUDCOMPARE_DB_LIBRARY_DEBUG
  NAMES QCC_DB_LIBd
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)

################################### 


###################################  
# CLOUDCOMPARE_GL

find_library(CLOUDCOMPARE_GL_LIBRARY
  NAMES QCC_GL_LIB
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)  
 
find_library(CLOUDCOMPARE_GL_LIBRARY_DEBUG
  NAMES QCC_GL_LIBd
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)   

################################### 


###################################

find_library(CLOUDCOMPARE_IO_LIBRARY
  NAMES QCC_IO_LIB
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)  

find_library(CLOUDCOMPARE_IO_LIBRARY_DEBUG
  NAMES QCC_IO_LIBd
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)  

################################### 
 
###################################

find_library(CLOUDCOMPARE_FBO_LIBRARY
  NAMES CC_FBO_LIB
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)  
find_library(CLOUDCOMPARE_FBO_LIBRARY_DEBUG
  NAMES CC_FBO_LIBd
  PATHS ${CLOUDCOMPARE_CHECK_LIBRARY_DIRS}
)  

################################### 

find_package_handle_standard_args(CloudCompare
    FOUND_VAR CLOUDCOMPARE_FOUND
    REQUIRED_VARS
       CLOUDCOMPARE_INCLUDE_DIR
       CLOUDCOMPARE_CORE_LIBRARY
       CLOUDCOMPARE_CORE_LIBRARY_DEBUG
       CLOUDCOMPARE_DB_LIBRARY
       CLOUDCOMPARE_DB_LIBRARY_DEBUG
       CLOUDCOMPARE_GL_LIBRARY
       CLOUDCOMPARE_GL_LIBRARY_DEBUG
       CLOUDCOMPARE_IO_LIBRARY
       CLOUDCOMPARE_IO_LIBRARY_DEBUG
       CLOUDCOMPARE_FBO_LIBRARY
       CLOUDCOMPARE_FBO_LIBRARY_DEBUG)
  
if(CLOUDCOMPARE_FOUND)
    set(CLOUDCOMPARE_LIBRARIES
      debug 
        ${CLOUDCOMPARE_CORE_LIBRARY_DEBUG}
      optimized 
        ${CLOUDCOMPARE_CORE_LIBRARY}
      debug
        ${CLOUDCOMPARE_DB_LIBRARY_DEBUG}
      optimized 
        ${CLOUDCOMPARE_DB_LIBRARY}
      debug
        ${CLOUDCOMPARE_GL_LIBRARY_DEBUG}
      optimized
        ${CLOUDCOMPARE_GL_LIBRARY}
      debug
        ${CLOUDCOMPARE_IO_LIBRARY_DEBUG}
      optimized
        ${CLOUDCOMPARE_IO_LIBRARY}
      debug
        ${CLOUDCOMPARE_FBO_LIBRARY_DEBUG}
      optimized
        ${CLOUDCOMPARE_FBO_LIBRARY})
		
    set(CLOUDCOMPARE_FOUND TRUE)
    message(STATUS "\n")
    message(STATUS "Found CloudCompare")
    message(STATUS "  Include : ${CLOUDCOMPARE_INCLUDE_DIR}")
    message(STATUS "  Library : ${CLOUDCOMPARE_CORE_LIBRARY}")
    message(STATUS "  Library : ${CLOUDCOMPARE_DB_LIBRARY}")
    message(STATUS "  Library : ${CLOUDCOMPARE_GL_LIBRARY}")
    message(STATUS "  Library : ${CLOUDCOMPARE_IO_LIBRARY}")
    message(STATUS "  Library : ${CLOUDCOMPARE_FBO_LIBRARY}")

    #TODO: Revisar esto
    if(MSVC)
      add_compile_definitions(CC_VIEWER)
      add_compile_definitions(CC_USE_AS_DLL)
      add_compile_definitions(QCC_DB_USE_AS_DLL)
      add_compile_definitions(QCC_IO_USE_AS_DLL)
    endif(MSVC)
	
else()
    if(CLOUDCOMPARE_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find CloudCompare")
    endif()
endif()