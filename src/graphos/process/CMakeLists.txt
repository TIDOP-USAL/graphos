include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_library(graphos_process ${LIB_TYPE}
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
            orientation/GeoreferenceProcess.cpp
            orientation/GeoreferenceProcess.h
            orientation/ImportOrientationProcess.cpp
            orientation/ImportOrientationProcess.h
            images/LoadImagesProcess.cpp
            images/LoadImagesProcess.h
            densification/DensificationProcess.cpp
            densification/DensificationProcess.h
            dtm/DtmProcess.cpp
            dtm/DtmProcess.h
            orthophoto/OrthophotoProcess.cpp
            orthophoto/OrthophotoProcess.h
           )

set_property(TARGET graphos_process PROPERTY FOLDER "lib")

target_link_libraries(graphos_process glog::glog)

install(TARGETS graphos_process
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/process
        FILES_MATCHING PATTERN "*.h")
