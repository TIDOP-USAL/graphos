include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_library(inspector_core ${LIB_TYPE}
            project.cpp
            project.h
            image.cpp
            image.h
            features/features.h
            features/featio.h
            features/featio.cpp
            #features/akaze.h
            #features/akaze.cpp
            features/sift.h
            features/sift.cpp
            utils.cpp
            utils.h)

set_property(TARGET inspector_core PROPERTY FOLDER "lib")

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
