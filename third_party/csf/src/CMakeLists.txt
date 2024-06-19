
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

project(CSF LANGUAGES CXX)

find_package(OpenMP)

add_library(${PROJECT_NAME} STATIC
            c2cdist.cpp
            c2cdist.h
            Cloth.cpp
            Cloth.h
            Constraint.cpp
            Constraint.h
            CSF.cpp
            CSF.h
            Particle.cpp
            Particle.h
            point_cloud.cpp
            point_cloud.h
            Rasterization.cpp
            Rasterization.h
            Vec3.h
            XYZReader.cpp
            XYZReader.h
)

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "third_party")

if(OpenMP_CXX_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)
    #target_compile_options(CSF PRIVATE -Werror -Wall -Wextra)
endif()

install(TARGETS ${PROJECT_NAME}
        EXPORT "${TARGETS_EXPORT_NAME}"
        LIBRARY DESTINATION lib 
        ARCHIVE DESTINATION lib)

install(DIRECTORY .
        DESTINATION include/csf
        FILES_MATCHING PATTERN "*.h")

set(CSF_LIBRARIES ${CSF_LIBRARIES} ${PROJECT_NAME})