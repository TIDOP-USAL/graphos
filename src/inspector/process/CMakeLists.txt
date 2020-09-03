include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_library(inspector_process ${LIB_TYPE}
            Process.cpp
            Process.h
            MultiProcess.cpp
            MultiProcess.h
            ExternalProcess.cpp
            ExternalProcess.h
            ProcessConcurrent.cpp
            ProcessConcurrent.h
            features/FeatureExtractorProcess.cpp
            features/FeatureExtractorProcess.h
            features/FeatureMatchingProcess.cpp
            features/FeatureMatchingProcess.h
            orientation/RelativeOrientationProcess.cpp
            orientation/RelativeOrientationProcess.h
			orientation/AbsoluteOrientationProcess.cpp
			orientation/AbsoluteOrientationProcess.h
            images/LoadImagesProcess.cpp
            images/LoadImagesProcess.h
            densification/DensificationProcess.cpp
            densification/DensificationProcess.h
           )

set_property(TARGET inspector_process PROPERTY FOLDER "lib")

target_link_libraries(inspector_process glog::glog)

install(TARGETS inspector_process
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/inspector/process
        FILES_MATCHING PATTERN "*.h")
