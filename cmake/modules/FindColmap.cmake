unset(COLMAP_FOUND)

SET(COLMAP_DIR "" CACHE PATH "Root folder of Colmap")


list(APPEND COLMAP_CHECK_INCLUDE_DIRS
     ${COLMAP_DIR}/include)
     
list(APPEND COLMAP_CHECK_LIBRARY_DIRS
     ${COLMAP_DIR}/lib/colmap)
	 
include(FindPackageHandleStandardArgs)
				
				
#colmap

find_path(COLMAP_INCLUDE_DIR
          NAMES colmap/util/version.h
          PATHS ${COLMAP_CHECK_INCLUDE_DIRS}
)
	
## flann

find_library(COLMAP_FLANN_LIBRARY
             NAMES flann
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_FLANN_LIBRARY_DEBUG
             NAMES flannd
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## graclus

find_library(COLMAP_GRACLUS_LIBRARY
             NAMES graclus
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_GRACLUS_LIBRARY_DEBUG
             NAMES graclusd
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## lsd

find_library(COLMAP_LSD_LIBRARY
             NAMES lsd
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_LSD_LIBRARY_DEBUG
             NAMES lsdd
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## pba

find_library(COLMAP_PBA_LIBRARY
             NAMES pba
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_PBA_LIBRARY_DEBUG
             NAMES pbad
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## poisson_recon

find_library(COLMAP_POISSON_RECON_LIBRARY
             NAMES poisson_recon
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_POISSON_RECON_LIBRARY_DEBUG
             NAMES poisson_recond
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## sqlite3

find_library(COLMAP_SQLITE3_LIBRARY
             NAMES sqlite3
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_SQLITE3_LIBRARY_DEBUG
             NAMES sqlite3d
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## sift_gpu

find_library(COLMAP_SIFT_GPU_LIBRARY
             NAMES sift_gpu
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_SIFT_GPU_LIBRARY_DEBUG
             NAMES sift_gpud
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## vlfeat

find_library(COLMAP_VLFEAT_LIBRARY
             NAMES vlfeat
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_VLFEAT_LIBRARY_DEBUG
             NAMES vlfeatd
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## colmap

find_library(COLMAP_LIBRARY
             NAMES colmap
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_LIBRARY_DEBUG
             NAMES colmapd
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

## colmap_cuda

find_library(COLMAP_CUDA_LIBRARY
             NAMES colmap_cuda
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)
find_library(COLMAP_CUDA_LIBRARY_DEBUG
             NAMES colmap_cudad
             PATHS ${COLMAP_CHECK_LIBRARY_DIRS}
)

find_package_handle_standard_args(COLMAP
  FOUND_VAR COLMAP_FOUND
  REQUIRED_VARS
    COLMAP_INCLUDE_DIR
	COLMAP_FLANN_LIBRARY
	COLMAP_FLANN_LIBRARY_DEBUG
	COLMAP_GRACLUS_LIBRARY
	COLMAP_GRACLUS_LIBRARY_DEBUG
	COLMAP_LSD_LIBRARY
	COLMAP_LSD_LIBRARY_DEBUG
	COLMAP_PBA_LIBRARY
	COLMAP_PBA_LIBRARY_DEBUG
	COLMAP_POISSON_RECON_LIBRARY
	COLMAP_POISSON_RECON_LIBRARY_DEBUG
	COLMAP_SQLITE3_LIBRARY
	COLMAP_SQLITE3_LIBRARY_DEBUG
	COLMAP_SIFT_GPU_LIBRARY
	COLMAP_SIFT_GPU_LIBRARY_DEBUG
	COLMAP_VLFEAT_LIBRARY
	COLMAP_VLFEAT_LIBRARY_DEBUG
	COLMAP_LIBRARY
	COLMAP_LIBRARY_DEBUG
	COLMAP_CUDA_LIBRARY
	COLMAP_CUDA_LIBRARY_DEBUG
)

if(COLMAP_FOUND)
  set(COLMAP_LIBRARIES 
      debug 
	    ${COLMAP_FLANN_LIBRARY_DEBUG}
      optimized 
	  	${COLMAP_FLANN_LIBRARY}
	  debug
	    ${COLMAP_GRACLUS_LIBRARY_DEBUG}
      optimized 
	    ${COLMAP_GRACLUS_LIBRARY}
      debug
		${COLMAP_LSD_LIBRARY_DEBUG}
	  optimized
		${COLMAP_LSD_LIBRARY}
      debug
		${COLMAP_PBA_LIBRARY_DEBUG}
	  optimized
		${COLMAP_PBA_LIBRARY}
      debug
		${COLMAP_POISSON_RECON_LIBRARY_DEBUG}
	  optimized
		${COLMAP_POISSON_RECON_LIBRARY}
      debug
		${COLMAP_SQLITE3_LIBRARY_DEBUG}
	  optimized
		${COLMAP_SQLITE3_LIBRARY}
      debug
		${COLMAP_SIFT_GPU_LIBRARY_DEBUG}
	  optimized
		${COLMAP_SIFT_GPU_LIBRARY}
      debug
		${COLMAP_VLFEAT_LIBRARY_DEBUG}
	  optimized
		${COLMAP_VLFEAT_LIBRARY}
      debug
		${COLMAP_LIBRARY_DEBUG}
	  optimized
		${COLMAP_LIBRARY}
      debug
		${COLMAP_CUDA_LIBRARY_DEBUG}
	  optimized
		${COLMAP_CUDA_LIBRARY})
		
  set(COLMAP_DEFINITIONS ${COLMAP_CFLAGS_OTHER})
  
    set(COLMAP_FOUND TRUE)
    message(STATUS "\n")
    message(STATUS "Found COLMAP")
    message(STATUS "  Include : ${COLMAP_INCLUDE_DIR}")
    message(STATUS "  Library : ${COLMAP_FLANN_LIBRARY}")
    message(STATUS "  Library : ${COLMAP_GRACLUS_LIBRARY}")
    message(STATUS "  Library : ${COLMAP_LSD_LIBRARY}")
    message(STATUS "  Library : ${COLMAP_PBA_LIBRARY}")
    message(STATUS "  Library : ${COLMAP_POISSON_RECON_LIBRARY}")
	message(STATUS "  Library : ${COLMAP_SQLITE3_LIBRARY}")
	message(STATUS "  Library : ${COLMAP_SIFT_GPU_LIBRARY}")
	message(STATUS "  Library : ${COLMAP_VLFEAT_LIBRARY}")
	message(STATUS "  Library : ${COLMAP_LIBRARY}")
	message(STATUS "  Library : ${COLMAP_CUDA_LIBRARY}")

#colmap dependencies


find_package(Ceres REQUIRED)

find_package(Boost REQUIRED
             COMPONENTS
               program_options
               filesystem
               regex
               system
               unit_test_framework)
			   
#find_package(Eigen3 REQUIRED)

find_package(FreeImage REQUIRED)
    
find_package(OpenGL REQUIRED)

find_package(GLEW REQUIRED)

list(APPEND COLMAP_INCLUDE_DIRS
     ${COLMAP_INCLUDE_DIR}
     ${COLMAP_INCLUDE_DIR}/colmap
     ${COLMAP_INCLUDE_DIR}/colmap/lib
	 ${Boost_INCLUDE_DIRS}
     ${EIGEN_INCLUDE_DIRS}
     #${Glog_INCLUDE_DIRS}
     # ${gflags_INCLUDE_DIRS}   
     ${FREEIMAGE_INCLUDE_DIRS}
     ${CERES_INCLUDE_DIRS}
     ${GLEW_INCLUDE_DIRS}
	 )

else()
    if(COLMAP_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find Colmap")
    endif()
endif()


# Dependencias
# - Eigen
# - Ceres
# - Boost
# - 