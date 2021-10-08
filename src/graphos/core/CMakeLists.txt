include_directories(${CMAKE_SOURCE_DIR}/third_party)

project(graphos_core LANGUAGES CXX)

add_library(${PROJECT_NAME} ${LIB_TYPE}
            ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/mvp.h
            ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/Component.h
            Application.cpp
            Application.h
            AppStatus.cpp
            AppStatus.h
            Component.cpp
            Component.h
            project.cpp
            project.h
            settings.cpp
            settings.h
            image.cpp
            image.h
            process/ProcessPresenter.cpp
            process/ProcessPresenter.h            
            process/ProcessView.cpp
            process/ProcessView.h
            process/Progress.cpp
            process/Progress.h
            features/features.h
            features/featio.h
            features/featio.cpp
            #features/akaze.h
            #features/akaze.cpp
            features/sift.h
            features/sift.cpp
            features/matching.cpp
            features/matching.h
            #orientation/orientation.cpp
            orientation/orientation.h
            orientation/orientationcolmap.cpp
            orientation/orientationcolmap.h
            orientation/orientationexport.cpp
            orientation/orientationexport.h
            orientation/poses.cpp
            orientation/poses.h
            orientation/posesio.cpp
            orientation/posesio.h
            orientation/gcp.cpp
            orientation/gcp.h
            densification/densification.h
            densification/CmvsPmvs.cpp
            densification/CmvsPmvs.h
            densification/Smvs.cpp
            densification/Smvs.h
            densification/DenseExport.cpp
            densification/DenseExport.h
            utils.cpp
            utils.h
            dtm/dtm.cpp
            dtm/dtm.h
            dtm/invdist.cpp
            dtm/invdist.h
            dtm/invdistnn.cpp
            dtm/invdistnn.h
            dtm/csf.h
            dtm/csf.cpp
            Orthophoto.cpp
            Orthophoto.h
            camera/colmap.cpp
            camera/colmap.h
            #camera/calibio.h
            #camera/calibio.cpp
            )
            
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "lib")

target_link_libraries(${PROJECT_NAME} PRIVATE glog::glog)

target_link_libraries(${PROJECT_NAME} PRIVATE CSF)

#if(BUILD_SHARED_LIBS)
#    
#  install(TARGETS libgraphos
#          RUNTIME DESTINATION bin
#          LIBRARY DESTINATION lib
#          ARCHIVE DESTINATION lib)
#	
#  set_target_properties(libgraphos PROPERTIES
#                        DEFINE_SYMBOL PHOTOMATCH_EXPORTS)
#	target_link_libraries(libprocess .... ) 
#else()
  install(TARGETS ${PROJECT_NAME}
          LIBRARY DESTINATION lib 
          ARCHIVE DESTINATION lib)
#endif()

install(DIRECTORY .
        DESTINATION include/graphos/core
        FILES_MATCHING PATTERN "*.h")
