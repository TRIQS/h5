#--------------------------------------------------------
#          Pure Python files
#--------------------------------------------------------

configure_file(version.py.in version.py)

# Copy and configure into the build dir to have a complete package for the tests.
file(GLOB_RECURSE python_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
foreach(file ${python_sources})
 configure_file(${file} ${file} COPYONLY)
endforeach()

# FIXME : FACTOR THIS ?
# FIXME was in cpp2py before, but it has nothing to do with c2py
# tried Python_SITEARCH e.g. from findPython, but this path is provided, it seems easier to use lib/python ... 
# The cumbersome part is dist/site ...
#
# Installation : Final destination of the python modules
 if(BUILD_DEBIAN_PACKAGE)
   set(PYTHON_LIB_DEST_DIR dist-packages)
 else()
   set(PYTHON_LIB_DEST_DIR site-packages)
 endif()
 set(PYTHON_LIB_DEST_ROOT lib/python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR}/${PYTHON_LIB_DEST_DIR}) # Install directory for Python Modules relative to CMAKE_INSTALL_PREFIX
 message(STATUS "Python modules will be installed in ${CMAKE_INSTALL_PREFIX}/${PYTHON_LIB_DEST_ROOT}")


# Install python files to proper location
set(PYTHON_LIB_DEST ${PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${PYTHON_LIB_DEST} FILES_MATCHING PATTERN "*.py")

#--------------------------------------------------------
#           Python Extension in C++ : _h5py module
#--------------------------------------------------------

option(GENERATE_PYTHON_BINDINGS "Use Clair python bindings generators" OFF)
if (GENERATE_PYTHON_BINDINGS)
  find_package(Clair REQUIRED)
endif()

# Some code not exposed to python
add_library(h5py_io OBJECT h5py_io.cpp)
target_link_libraries(h5py_io PUBLIC h5_c hdf5 c2py::c2py)
target_include_directories(h5py_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# FIXME : not needed as it is static, right ?
#set_property(TARGET h5py_io PROPERTY POSITION_INDEPENDENT_CODE ON)

# The python module
set(module_name _h5py)

# Compile it
Python_add_library(${module_name} MODULE ${module_name}.$<IF:$<BOOL:${GENERATE_PYTHON_BINDINGS}>,cpp,wrap.cxx>)
target_include_directories(${module_name} SYSTEM PRIVATE  ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${module_name}             PRIVATE  h5py_io   c2py::c2py    $<$<BOOL:${GENERATE_PYTHON_BINDINGS}>:clair::c2py_plugin>)
target_link_libraries(${module_name}             PRIVATE $<$<BOOL:${ASAN}>:$<BUILD_INTERFACE:asan>> $<$<BOOL:${UBSAN}>:$<BUILD_INTERFACE:ubsan>> )

# Alias
add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})

# Installation
install(TARGETS ${module_name} DESTINATION ${PYTHON_LIB_DEST}/${module_dir})

# FIXME : remove if previous line is ok.
#if(ASAN)
  #target_link_libraries(${module_name}  PRIVATE $<BUILD_INTERFACE:asan>)
#endif()

#if(UBSAN)
  #target_link_libraries(${module_name}  PRIVATE $<BUILD_INTERFACE:ubsan>)
#endif()

#--------------------------------------------------------
#  Developer only
#  A target to copy the wrap.cxx files back to the src
#--------------------------------------------------------

add_custom_target(clair_copy_bindings_to_src_dir)
add_custom_command(
  TARGET clair_copy_bindings_to_src_dir
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/*.wrap.cxx     ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/*.wrap.hxx     ${CMAKE_SOURCE_DIR}/c++/${PROJECT_NAME}
  COMMAND cp ${CMAKE_BINARY_DIR}/test/python/*.wrap.?xx ${CMAKE_SOURCE_DIR}/test/python
)

