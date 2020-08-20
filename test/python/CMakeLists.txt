# Copy h5 files to binary dir
file(GLOB_RECURSE all_h5_ref_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.ref.h5)
foreach(file ${all_h5_ref_files})
  configure_file(${file} ${file} COPYONLY)
endforeach()

# List of all tests
set(all_tests interface archive test_storable test_iterable issue_multifile)

# Generate the C++2Py Module for an iterable class
# FIXME : MOVE THIS TEST
pybind11_add_module(iterable iterable.cxx)
#
# Generate the C++2Py Module for an hdf5 storable class
pybind11_add_module(storable storable.cxx)
#target_include_directories(storable PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(storable PRIVATE  h5_c hdf5) 
target_compile_options(storable PUBLIC $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>)


foreach(test ${all_tests})
  get_filename_component(test_name ${test} NAME_WE)
  get_filename_component(test_dir ${test} DIRECTORY)
  add_test(NAME Py_${test_name} COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${test_dir}/${test_name}.py WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_dir})
  set_property(TEST Py_${test_name} APPEND PROPERTY ENVIRONMENT PYTHONPATH=${PROJECT_BINARY_DIR}/python:${CMAKE_CURRENT_BINARY_DIR}:$ENV{PYTHONPATH} ${SANITIZER_RT_PRELOAD})
endforeach()
