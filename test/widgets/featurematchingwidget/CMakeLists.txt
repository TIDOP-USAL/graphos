set(test_filename tst_featurematchingwidget.cpp)
get_filename_component(test_name ${test_filename} NAME_WE)
DEFINE_TEST_WIDGETS(${test_name} ${test_filename})
