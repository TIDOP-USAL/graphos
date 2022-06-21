#FreeImage

unset(FREEIMAGE_FOUND)

SET(FREEIMAGE_ROOT "" CACHE PATH "Root folder of FREEIMAGE")

list(APPEND FREEIMAGE_INCLUDE_DIRS 
     ${FREEIMAGE_ROOT}/include)
     
list(APPEND FREEIMAGE_LIBRARY_DIRS 
     ${FREEIMAGE_ROOT}/lib)
     
if(CMAKE_CL_64)
    list(APPEND FREEIMAGE_LIBRARY_DIRS ${FREEIMAGE_ROOT}/lib/x64)
else()
    list(APPEND FREEIMAGE_LIBRARY_DIRS ${FREEIMAGE_ROOT}/lib/x32)
endif()

include(FindPackageHandleStandardArgs)


find_path(FREEIMAGE_INCLUDE_DIR
          NAMES FreeImage.h
          PATHS ${FREEIMAGE_INCLUDE_DIRS}
)

find_library(FREEIMAGE_LIBRARY
             NAMES FreeImage
             PATHS ${FREEIMAGE_LIBRARY_DIRS}
)

find_package_handle_standard_args(FREEIMAGE
                                  FOUND_VAR FREEIMAGE_FOUND
                                  REQUIRED_VARS
                                    FREEIMAGE_LIBRARY
                                    FREEIMAGE_INCLUDE_DIR
)

if(FREEIMAGE_FOUND)
  set(FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_INCLUDE_DIR})
  set(FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY})
endif()


if (FREEIMAGE_INCLUDE_DIR AND FREEIMAGE_LIBRARY)
    message(STATUS "\n")
    message(STATUS "Found FREEIMAGE")
    message(STATUS "  Include   : ${FREEIMAGE_INCLUDE_DIR}")
    message(STATUS "  Library   : ${FREEIMAGE_LIBRARY}")
else()
    if(FREEIMAGE_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find FREEIMAGE")
    endif()
endif()