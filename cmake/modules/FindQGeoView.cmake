#QGeoView

unset(QGeoView_FOUND)

SET(QGeoView_ROOT "" CACHE PATH "Root folder of QGeoView")

list(APPEND QGeoView_INCLUDE_DIRS 
    /opt/local/include
    /usr/local/include
    /usr/include	
    ${QGeoView_ROOT}/include)
     
list(APPEND QGeoView_LIBRARY_DIRS 
    /opt/local/lib
    /usr/local/lib
    /usr/lib
    ${QGeoView_ROOT}/lib
    ${QGeoView_ROOT}/lib/static)

include(FindPackageHandleStandardArgs)


find_path(QGeoView_INCLUDE_DIR
          NAMES QGeoView/QGVGlobal.h
          PATHS ${QGeoView_INCLUDE_DIRS}
)

find_library(QGeoView_LIBRARY
             NAMES qgeoview
             PATHS ${QGeoView_LIBRARY_DIRS}
)

find_library(QGeoView_LIBRARY_DEBUG
             NAMES qgeoviewd
             PATHS ${QGeoView_LIBRARY_DIRS}
)

find_package_handle_standard_args(QGeoView
                                  FOUND_VAR QGeoView_FOUND
                                  REQUIRED_VARS
                                    QGeoView_LIBRARY
									QGeoView_LIBRARY_DEBUG
                                    QGeoView_INCLUDE_DIR
)

if(QGeoView_FOUND)
  set(QGeoView_INCLUDE_DIRS ${QGeoView_INCLUDE_DIR})
  set(QGeoView_LIBRARIES debug ${QGeoView_LIBRARY_DEBUG} 
                     optimized ${QGeoView_LIBRARY})
else()
  set(QGeoView_INCLUDE_DIR "")
  set(QGeoView_LIBRARY_RELEASE "")
  set(QGeoView_LIBRARY_DEBUG "")
endif()


if (QGeoView_INCLUDE_DIR AND QGeoView_LIBRARY)
    message(STATUS "\n")
    message(STATUS "Found QGeoView")
    message(STATUS "  Include   : ${QGeoView_INCLUDE_DIRS}")
    message(STATUS "  Library   : ${QGeoView_LIBRARY}")
else()
    if(QGeoView_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find QGeoView")
    endif()
endif()