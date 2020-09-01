# Configure the version
configure_file(version.py.in version.py)

# All Python files. Copy them in the build dir to have a complete package for the tests.
file(GLOB_RECURSE python_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
file(GLOB_RECURSE wrap_generators RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *_desc.py)
list(REMOVE_ITEM python_sources "${wrap_generators}")
foreach(file ${python_sources})
  configure_file(${file} ${file} COPYONLY)
endforeach()

find_package(Python3 COMPONENTS Interpreter NumPy)

# Build the pybind11 module
pybind11_add_module(_h5py _h5py.cpp h5py_io.cpp)
target_link_libraries(_h5py PRIVATE h5_c hdf5 Python3::NumPy)
target_compile_options(_h5py PUBLIC $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>)

# Install the files to proper location
set(PYTHON_LIB_DEST ${Python3_SITELIB}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST})
install(TARGETS _h5py DESTINATION ${PYTHON_LIB_DEST})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${Python3_SITELIB} FILES_MATCHING PATTERN "*.py" PATTERN "*_desc.py" EXCLUDE)
