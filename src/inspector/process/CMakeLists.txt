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
            images/LoadImagesProcess.cpp
            images/LoadImagesProcess.h
           )

set_property(TARGET inspector_process PROPERTY FOLDER "lib")


install(TARGETS inspector_process
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/inspector/process
        FILES_MATCHING PATTERN "*.h")
