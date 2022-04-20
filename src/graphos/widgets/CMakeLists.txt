include_directories(${CMAKE_SOURCE_DIR}/third_party)

project(graphos_widgets LANGUAGES CXX)

add_library(${PROJECT_NAME} ${LIB_TYPE}
            GraphosWidget.h
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
            MvsWidget.cpp
            MvsWidget.h
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
            OriTxtFormatWidget.h
            GraphicItem.cpp
            GraphicItem.h
            GraphicViewer.cpp
            GraphicViewer.h
            Viewer3d.cpp
            Viewer3d.h)

target_link_libraries(${PROJECT_NAME}
                      TidopLib::tl_core
                      TidopLib::tl_geom
                      TidopLib::tl_geospatial
                      TidopLib::tl_img
                      TidopLib::tl_math
                      TidopLib::tl_vect
                      TidopLib::tl_graphic)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "lib")

install(TARGETS ${PROJECT_NAME} 
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/widgets
        FILES_MATCHING PATTERN "*.h")
