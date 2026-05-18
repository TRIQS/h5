# List of all tests
set(all_tests interface archive test_storable issue_multifile)

# Build the storable C++ module
Python_add_library(storable MODULE storable.cpp)
target_link_libraries(storable PRIVATE h5_c c2py::c2py)

if(Update_Python_Bindings)
  clair_c2py_generate_bindings(storable)
  add_dependencies(storable_bindings_generation _h5py)
endif()

# Add tests
find_package(Python COMPONENTS Interpreter)
foreach(test ${all_tests})
  get_filename_component(test_name ${test} NAME_WE)
  get_filename_component(test_dir ${test} DIRECTORY)
  add_test(NAME Py_${test_name} COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${test_dir}/${test_name}.py WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_dir})
  set_property(TEST Py_${test_name} APPEND PROPERTY ENVIRONMENT PYTHONPATH=${PROJECT_BINARY_DIR}/python:${h5_BINARY_DIR}/python:${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH} ${SANITIZER_RT_PRELOAD})
endforeach()
