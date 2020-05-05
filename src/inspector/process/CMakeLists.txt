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
            #preprocess/ImagePreprocess.cpp
            #preprocess/ImagePreprocess.h
            #Matching/MatchingProcess.cpp
            #Matching/MatchingProcess.h
            #Matching/PassPointsGroupingProcess.cpp
            #Matching/PassPointsGroupingProcess.h
           )

set_property(TARGET inspector_process PROPERTY FOLDER "lib")


install(TARGETS inspector_process
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/inspector/process
        FILES_MATCHING PATTERN "*.h")
