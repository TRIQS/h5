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

# Option to update/generate the C++ Python bindings using clair-c2py
option(Update_Python_Bindings "Optionally update the C++ Python bindings in the source directory using clair-c2py" ON)

# Add Python module
set(module_name _h5py)
Python_add_library(${module_name} MODULE ${module_name}.cpp)
add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})
target_link_libraries(${module_name} PRIVATE ${PROJECT_NAME}_c ${PROJECT_NAME}_warnings c2py::c2py h5py_io)
set_target_properties(${module_name} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${module_dir}
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
  
# Optionally generate Python bindings with clair-c2py
if(Update_Python_Bindings)
  clair_c2py_generate_bindings(${module_name})
endif()

# Install the Python module and the generated header
install(TARGETS ${module_name} DESTINATION ${PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME}/${module_dir})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${module_name}.wrap.hxx DESTINATION include/${PROJECT_NAME})
