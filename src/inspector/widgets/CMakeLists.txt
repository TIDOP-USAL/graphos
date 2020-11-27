include_directories(${CMAKE_SOURCE_DIR}/third_party)

add_library(inspector_widgets ${LIB_TYPE}
            InspectorWidget.h
            SiftWidget.cpp
            SiftWidget.h
            ThumbnailsWidget.cpp
            ThumbnailsWidget.h
            LogWidget.cpp
            LogWidget.h
            StartPageWidget.cpp
            StartPageWidget.h
            FeatureMatchingWidget.cpp
            FeatureMatchingWidget.h
            SmvsWidget.cpp
            SmvsWidget.h
            CmvsPmvsWidget.cpp
            CmvsPmvsWidget.h
            NvmFormatWidget.cpp
            NvmFormatWidget.h
            BundlerFormatWidget.cpp
            BundlerFormatWidget.h
            MveFormatWidget.cpp
            MveFormatWidget.h
            PointCloudCSVFormatWidget.cpp
            PointCloudCSVFormatWidget.h
            DtmInvDistWidget.cpp
            DtmInvDistWidget.h
            DtmInvDistNNWidget.cpp
            DtmInvDistNNWidget.h
            OriTxtFormatWidget.cpp
            OriTxtFormatWidget.h)

set_property(TARGET inspector_widgets PROPERTY FOLDER "lib")

install(TARGETS inspector_widgets 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/inspector/widgets
        FILES_MATCHING PATTERN "*.h")
