add_subdirectory(core)
add_subdirectory(widgets)
add_subdirectory(components)
add_subdirectory(gui)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/graphos_global.h"
        DESTINATION include/graphos)