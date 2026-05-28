# Configure the version
configure_file(version.py.in version.py)

# Copy Python files into the build dir to have a complete package for the tests
file(GLOB_RECURSE python_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
foreach(file ${python_sources})
  configure_file(${file} ${file} COPYONLY)
endforeach()

# Install python files to proper location
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${PYTHON_LIB_DEST_ROOT} FILES_MATCHING PATTERN "*.py")

# Build the h5py_io library
add_library(h5py_io STATIC h5py_io.cpp)
set_property(TARGET h5py_io PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(h5py_io PUBLIC ${PROJECT_NAME}_c hdf5::hdf5 c2py::c2py)
target_include_directories(h5py_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Add the _h5py Python module
c2py_add_module(_h5py LINK_LIBRARIES ${PROJECT_NAME}_c ${PROJECT_NAME}_warnings h5py_io)
add_library(${PROJECT_NAME}::_h5py ALIAS _h5py)

# Install the Python module and the generated header
install(TARGETS _h5py DESTINATION ${PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/_h5py.wrap.hxx DESTINATION include/${PROJECT_NAME})
