include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_library(inspector_core ${LIB_TYPE}
            project.cpp
            project.h
            settings.cpp
            settings.h
            image.cpp
            image.h
            camera.cpp
            camera.h
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
            orientation/photoorientation.cpp
            orientation/photoorientation.h
            densification/densification.h
            densification/CmvsPmvs.cpp
            densification/CmvsPmvs.h
            densification/Smvs.cpp
            densification/Smvs.h
            utils.cpp
            utils.h)

set_property(TARGET inspector_core PROPERTY FOLDER "lib")

target_link_libraries(inspector_core glog::glog)

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
  install(TARGETS inspector_core
          LIBRARY DESTINATION lib 
          ARCHIVE DESTINATION lib)
#endif()

install(DIRECTORY .
        DESTINATION include/inspector/core
        FILES_MATCHING PATTERN "*.h")
