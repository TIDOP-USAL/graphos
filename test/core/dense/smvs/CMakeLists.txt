##########################################################################
#                                                                        #
# Copyright 2016 by Tidop Research Group <daguilera@usal.es>             #
#                                                                        #
# This file is part of GRAPHOS - inteGRAted PHOtogrammetric Suite.       #
#                                                                        #
# GRAPHOS - inteGRAted PHOtogrammetric Suite is free software: you can   #
# redistribute it and/or modify it under the terms of the GNU General    #
# Public License as published by the Free Software Foundation, either    #
# version 3 of the License, or (at your option) any later version.       #
#                                                                        #
# GRAPHOS - inteGRAted PHOtogrammetric Suite is distributed in the       #
# hope that it will be useful, but WITHOUT ANY WARRANTY; without even    #
# the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR    #
# PURPOSE.  See the GNU General Public License for more details.         #
#                                                                        #
# You should have received a copy of the GNU General Public License      #
# along with Graphos.  If not, see <http://www.gnu.org/licenses/>.       #
#                                                                        #
# https://spdx.org/licenses/GPL-3.0-or-later.html                        #
#                                                                        #
##########################################################################

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

set(test_filename tst_smvs.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
set(test_target "${test_name}")

add_executable(${test_target} ${test_filename})

target_link_libraries(${test_target} PRIVATE
                      Graphos::Core
                      TidopLib::Core
                      TidopLib::Img
                      TidopLib::Geospatial
                      Qt5::Core
                      Qt5::Test
                      Qt5::Sql
                      ${COLMAP_LIBRARIES}
                      ${CUDA_LIBRARIES}
                      ${FREEIMAGE_LIBRARIES}
                      GLEW::GLEW
                      ${CERES_LIBRARIES}
                      ${OpenCV_LIBS}
                      ${GDAL_LIBRARY}
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

set_target_properties(${test_target} PROPERTIES
                      OUTPUT_NAME ${test_target}
                      PROJECT_LABEL ${test_name})

set_target_properties(${test_target} PROPERTIES 
	                  FOLDER "test/core/dense")

add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
