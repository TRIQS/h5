# Configure the version
configure_file(version.py.in version.py)

# All Python files. Copy them in the build dir to have a complete package for the tests.
file(GLOB_RECURSE python_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
file(GLOB_RECURSE wrap_generators RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *_desc.py)
list(REMOVE_ITEM python_sources "${wrap_generators}")
foreach(file ${python_sources})
  configure_file(${file} ${file} COPYONLY)
endforeach()

# Install python files to proper location
set(PYTHON_LIB_DEST ${PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${PYTHON_LIB_DEST_ROOT} FILES_MATCHING PATTERN "*.py" PATTERN "*_desc.py" EXCLUDE)

# Build the h5py_io library
#add_library(h5py_io STATIC h5py_io.cpp)
#set_property(TARGET h5py_io PROPERTY POSITION_INDEPENDENT_CODE ON)
#target_link_libraries(h5py_io PUBLIC ${PROJECT_NAME}_c hdf5 Python3::Module Python3::NumPy) #cpp2py::cpp2py cpp2py::python_and_numpy)
#target_include_directories(h5py_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Build the cpp2py module
#add_cpp2py_module(_h5py)
#target_link_libraries(_h5py h5py_io)

## Build the pybind11 module
pybind11_add_module(_h5py _h5py.cpp h5py_io.cpp)
target_link_libraries(_h5py PRIVATE  h5_c hdf5) 
target_compile_options(_h5py PUBLIC $<$<CXX_COMPILER_ID:GNU>:-Wno-attributes>)
install(TARGETS _h5py DESTINATION ${PYTHON_LIB_DEST})

## Build and install any python modules
#foreach(gen ${wrap_generators})
  #string(REPLACE "_desc.py" "" gen ${gen})
  #get_filename_component(module_name ${gen} NAME_WE)
  #get_filename_component(module_dir ${gen} DIRECTORY)
  #add_cpp2py_module(NAME ${module_name} DIRECTORY ${module_dir})
  #add_library(h5::${module_name} ALIAS ${module_name})
  #target_link_libraries(${module_name} h5py_io)
  #install(TARGETS ${module_name} DESTINATION ${PYTHON_LIB_DEST}/${module_dir})
#endforeach()
