# List of all tests
set(all_tests h5_interface hdf5_io hdf5_class)

# Generate the C++2Py Module for an hdf5 storable class
add_cpp2py_module(storable)
target_link_libraries(storable PRIVATE h5_interface)
target_include_directories(storable PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

foreach(test ${all_tests})
  get_filename_component(test_name ${test} NAME_WE)
  get_filename_component(test_dir ${test} DIRECTORY)
  add_test(NAME Py_${test_name} COMMAND ${PYTHON_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/${test_dir}/${test_name}.py WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_dir})
  set_property(TEST Py_${test_name} APPEND PROPERTY ENVIRONMENT PYTHONPATH=${PROJECT_BINARY_DIR}/python:${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH} ${SANITIZER_RT_PRELOAD})
endforeach()
