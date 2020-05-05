add_subdirectory(core)
#add_subdirectory(process)

if (BUILD_INSPECTOR_IMAGE)
add_subdirectory(ui)
add_subdirectory(widgets)
endif (BUILD_INSPECTOR_IMAGE)