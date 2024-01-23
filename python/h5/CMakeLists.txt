# Configure the version
configure_file(version.py.in version.py)

# ------- Python files ----------

# Copy them in the build dir to have a complete package for the tests.
file(GLOB_RECURSE python_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)
foreach(file ${python_sources})
 configure_file(${file} ${file} COPYONLY)
endforeach()

# FIXME : FACTOR THIS ?
# Installation : Final destination of the python modules
 if(BUILD_DEBIAN_PACKAGE)
   set(PYTHON_LIB_DEST_DIR dist-packages)
 else()
   set(PYTHON_LIB_DEST_DIR site-packages)
 endif()
 set(PYTHON_LIB_DEST_ROOT lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/${PYTHON_LIB_DEST_DIR} CACHE PATH "Install directory for Python Modules relative to CMAKE_INSTALL_PREFIX")
 message(STATUS "Python modules will be installed in ${CMAKE_INSTALL_PREFIX}/${PYTHON_LIB_DEST_ROOT}")

# Install python files to proper location
set(PYTHON_LIB_DEST ${PYTHON_LIB_DEST_ROOT}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/version.py DESTINATION ${PYTHON_LIB_DEST})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${PYTHON_LIB_DEST} FILES_MATCHING PATTERN "*.py")

# ------- Python Extension ----------
# 
option(GENERATE_PYTHON_BINDINGS "Use Clair python bindings generators" OFF)
if (GENERATE_PYTHON_BINDINGS)
  find_package(Clair REQUIRED)
endif()

## Build the h5py_io library
add_library(h5py_io STATIC h5py_io.cpp)
set_property(TARGET h5py_io PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(h5py_io PUBLIC ${PROJECT_NAME}_c hdf5 c2py::c2py)
target_include_directories(h5py_io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(module_name _h5py)

Python_add_library(${module_name} MODULE ${module_name}.$<IF:$<BOOL:${GENERATE_PYTHON_BINDINGS}>,cpp,wrap.cxx>)

target_include_directories(${module_name} SYSTEM PRIVATE  ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${module_name} PRIVATE ${PROJECT_NAME}_c hdf5 h5py_io c2py::c2py $<$<BOOL:${GENERATE_PYTHON_BINDINGS}>:clair::c2py_plugin>)

# What is the use of this ? 
add_library(${PROJECT_NAME}::${module_name} ALIAS ${module_name})

install(TARGETS ${module_name} DESTINATION ${PYTHON_LIB_DEST}/${module_dir})

if(ASAN)
  target_link_libraries(${module_name}  PRIVATE $<BUILD_INTERFACE:asan>)
endif()

if(UBSAN)
  target_link_libraries(${module_name}  PRIVATE $<BUILD_INTERFACE:ubsan>)
endif()

#--------------------------------------------------------
#  A target to copy the wrap.cxx files back to the src
#--------------------------------------------------------

add_custom_target(copy_wrap_cxx_to_src)
add_custom_command(
  TARGET copy_wrap_cxx_to_src
  COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/*.wrap.cxx ${CMAKE_CURRENT_BINARY_DIR}/*.wrap.hxx ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND echo "** Copying all wrap.[ch]xx to src"
)

