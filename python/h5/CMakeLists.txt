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

# ---------------------------------------------------
# h5py_c : the part of h5_c that uses python, and is linked to it
# It contains in particular the function to h5 de/serialize
# into a PyByteArray 
# ---------------------------------------------------
add_library(h5py_c h5py_c.cpp)
add_library(h5::h5py_c ALIAS h5_c)

target_link_libraries(h5py_c PRIVATE $<BUILD_INTERFACE:${PROJECT_NAME}_warnings>)
target_link_libraries(h5py_c PUBLIC h5_c hdf5 Python3::NumPy)

# Set C++ Standard
if(HAS_CPP20)
  target_compile_options(h5py_c PUBLIC -std=c++20)
else()
  target_compile_options(h5py_c PUBLIC -std=c++17 $<$<CXX_COMPILER_ID:GNU>:-fconcepts>)
endif()

# Configure target and compilation
set_property(TARGET h5py_c PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(h5py_c PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/c++>)
target_include_directories(h5py_c SYSTEM INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions(h5py_c PUBLIC
				H5_GIT_HASH=${PROJECT_GIT_HASH}
				$<$<CONFIG:Debug>:H5_DEBUG>
			  )

# Install library and headers
install(TARGETS h5py_c EXPORT ${PROJECT_NAME}-targets DESTINATION lib)


# ---------------------------------------------------
# python module
# ---------------------------------------------------

# Build the pybind11 module
pybind11_add_module(_h5py _h5py.cpp h5py_io.cpp)
target_link_libraries(_h5py PRIVATE h5_c hdf5 Python3::NumPy)
target_compile_options(_h5py PUBLIC $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>)


# Install the files to proper location
set(PYTHON_LIB_DEST ${Python3_SITELIB}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST})
install(TARGETS _h5py DESTINATION ${PYTHON_LIB_DEST})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${Python3_SITELIB} FILES_MATCHING PATTERN "*.py" PATTERN "*_desc.py" EXCLUDE)
