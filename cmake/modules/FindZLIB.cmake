#ZLIB

unset(ZLIB_FOUND)

SET(ZLIB_DIR "" CACHE PATH "Root folder of ZLIB")

list(APPEND ZLIB_INCLUDE_DIRS 
     ${ZLIB_DIR}/include)
     
list(APPEND ZLIB_LIBRARY_DIRS 
     ${ZLIB_DIR}/lib
	 ${ZLIB_DIR}/debug/lib)

include(FindPackageHandleStandardArgs)


find_path(ZLIB_INCLUDE_DIR
          NAMES zlib.h
          PATHS ${ZLIB_INCLUDE_DIRS}
)

find_library(ZLIB_LIBRARY
             NAMES zlib
             PATHS ${ZLIB_LIBRARY_DIRS}
)

find_library(ZLIB_LIBRARY_DEBUG
             NAMES zlibd
             PATHS ${ZLIB_LIBRARY_DIRS}
)

find_package_handle_standard_args(ZLIB
                                  FOUND_VAR ZLIB_FOUND
                                  REQUIRED_VARS
                                    ZLIB_LIBRARY
									ZLIB_LIBRARY_DEBUG
                                    ZLIB_INCLUDE_DIR
)

if(ZLIB_FOUND)
  set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})
  set(ZLIB_LIBRARIES debug ${ZLIB_LIBRARY_DEBUG} 
                     optimized ${ZLIB_LIBRARY})
else()
  set(ZLIB_INCLUDE_DIR "")
  set(ZLIB_LIBRARY_RELEASE "")
  set(ZLIB_LIBRARY_DEBUG "")
endif()


if (ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    message(STATUS "\n")
    message(STATUS "Found ZLIB")
    message(STATUS "  Include   : ${ZLIB_INCLUDE_DIR}")
    message(STATUS "  Library   : ${ZLIB_LIBRARY}")
else()
    if(ZLIB_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find ZLIB")
    endif()
endif()