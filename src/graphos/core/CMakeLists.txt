include_directories(${CMAKE_SOURCE_DIR}/third_party)

project(graphos_core LANGUAGES CXX)

set(GRAPHOS_CORE_SOURCES
    Application.cpp
    AppStatus.cpp
    Component.cpp
    project.cpp
    settings.cpp
    image.cpp
    Pdf.cpp
    process/ProcessPresenter.cpp         
    process/ProcessView.cpp
    process/Progress.cpp
    features/featio.cpp
    features/sift.cpp
    features/matching.cpp
    orientation/orientationcolmap.cpp
    orientation/orientationexport.cpp
    orientation/poses.cpp
    orientation/posesio.cpp
    orientation/gcp.cpp
    densification/CmvsPmvs.cpp
    densification/Smvs.cpp
    densification/DenseExport.cpp
    utils.cpp
    dtm/dtm.cpp
    dtm/invdist.cpp
    dtm/invdistnn.cpp
    dtm/csf.cpp
    camera/Calibration.cpp
    camera/Camera.cpp
    camera/Colmap.cpp
    camera/Database.cpp
    camera/Utils.cpp
    ortho/Orthorectification.cpp
    ortho/ZBuffer.cpp
    ortho/Footprint.cpp
    ortho/Orthoimage.cpp
    ortho/Orthomosaic.cpp)
			
set(GRAPHOS_CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/mvp.h
    ${CMAKE_SOURCE_DIR}/src/graphos/interfaces/Component.h
    Application.h
    AppStatus.h
    Component.h
    command.h
    project.h
    settings.h
    image.h
    Pdf.h
    process/ProcessPresenter.h            
    process/ProcessView.h
    process/Progress.h
    features/features.h
    features/featio.h
    features/sift.h
    features/matching.h
    orientation/orientation.h
    orientation/orientationcolmap.h
    orientation/orientationexport.h
    orientation/poses.h
    orientation/posesio.h
    orientation/gcp.h
    densification/densification.h
    densification/CmvsPmvs.h
    densification/Smvs.h
    densification/DenseExport.h
    utils.h
    dtm/dtm.h
    dtm/invdist.h
    dtm/invdistnn.h
    dtm/csf.h
    camera/Calibration.h
    camera/Camera.h
    camera/Colmap.h
    camera/Database.h
    camera/Utils.h
    ortho/Orthorectification.h
    ortho/ZBuffer.h
    ortho/Footprint.h
    ortho/Orthoimage.h
    ortho/Orthomosaic.h)
	
add_library(${PROJECT_NAME} ${LIB_TYPE}
            ${GRAPHOS_CORE_SOURCES}
            ${GRAPHOS_CORE_HEADERS})
            
foreach(_source IN ITEMS ${GRAPHOS_CORE_SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Source Files\\${_group_path}" FILES "${_source}")
endforeach()

foreach(_source IN ITEMS ${GRAPHOS_CORE_HEADERS})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "/" "\\" _group_path "${_source_path}")
    source_group("Header Files\\${_group_path}" FILES "${_source}")
endforeach()


set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "lib")

target_link_libraries(${PROJECT_NAME} PRIVATE
                      CSF
                      glog::glog
                      Qt5::PrintSupport
                      Eigen3::Eigen
                      Ceres::ceres)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/graphos/core
        FILES_MATCHING PATTERN "*.h")
