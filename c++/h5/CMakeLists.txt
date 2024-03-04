file(GLOB_RECURSE sources *.cpp)
add_library(${PROJECT_NAME}_c ${sources})
add_library(${PROJECT_NAME}::${PROJECT_NAME}_c ALIAS ${PROJECT_NAME}_c)

# Enable warnings
target_link_libraries(${PROJECT_NAME}_c PRIVATE $<BUILD_INTERFACE:${PROJECT_NAME}_warnings>)

# Configure target and compilation
target_compile_features(${PROJECT_NAME}_c PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME}_c PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_include_directories(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/c++>)
target_include_directories(${PROJECT_NAME}_c SYSTEM INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_FULL_INCLUDEDIR}>)
target_compile_definitions(${PROJECT_NAME}_c PUBLIC
				H5_GIT_HASH=${PROJECT_GIT_HASH}
				$<$<CONFIG:Debug>:H5_DEBUG>
			  )

# Install library and headers
install(TARGETS ${PROJECT_NAME}_c EXPORT ${PROJECT_NAME}-targets DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h" PATTERN "*.wrap.hxx")

# ========= Additional Depdencies ==========

message(STATUS "-------- HDF5 detection -------------")

find_package(HDF5 REQUIRED C HL)

# Create an interface target
add_library(hdf5 INTERFACE)
add_library(h5::hdf5 ALIAS hdf5)
target_include_directories(hdf5 SYSTEM INTERFACE ${HDF5_INCLUDE_DIRS})
if(TARGET hdf5::hdf5 AND TARGET hdf5::hdf5_hl)
  # Can't directly link against imported targets as they are not exportable
  # Instead extract information on library locations
  get_target_property(HDF5_LIBRARIES hdf5::hdf5 IMPORTED_LOCATION)
  get_target_property(HDF5_HL_LIBRARIES hdf5::hdf5_hl IMPORTED_LOCATION)
  # Starting with cmake 3.23 we may need to use IMPORTED_LOCATION_RELEASE property
  if(NOT HDF5_LIBRARIES)
    get_target_property(HDF5_LIBRARIES hdf5::hdf5 IMPORTED_LOCATION_RELEASE)
    get_target_property(HDF5_HL_LIBRARIES hdf5::hdf5_hl IMPORTED_LOCATION_RELEASE)
  endif()
endif()
target_link_libraries(hdf5 INTERFACE "${HDF5_LIBRARIES}" ${HDF5_HL_LIBRARIES})
target_compile_options(hdf5 INTERFACE ${HDF5_DEFINITIONS})
if(HDF5_VERSION VERSION_GREATER 1.10)
  target_compile_definitions(hdf5 INTERFACE H5_USE_110_API)
endif()
if(HDF5_VERSION VERSION_GREATER_EQUAL 1.13)
  target_compile_definitions(hdf5 INTERFACE H5_VER_GE_113)
endif()

# Link against interface target and export
target_link_libraries(h5_c PRIVATE hdf5)
install(TARGETS hdf5 EXPORT h5-targets)


# ========= Static Analyzer Checks ==========

option(ANALYZE_SOURCES OFF "Run static analyzer checks if found (clang-tidy, cppcheck)")
if(ANALYZE_SOURCES)

  # Locate static analyzer tools
  find_program(CPPCHECK_EXECUTABLE NAMES "cppcheck" PATHS ENV PATH)
  find_program(CLANG_TIDY_EXECUTABLE NAMES "clang-tidy" PATHS ENV PATH)

  # Run clang-tidy if found
  if(CLANG_TIDY_EXECUTABLE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXECUTABLE}")
    set_target_properties(${PROJECT_NAME}_c PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
  else()
    message(STATUS "clang-tidy not found in $PATH. Please consider installing clang-tidy for additional checks!")
  endif()

  # Run cppcheck if found
  if(CPPCHECK_EXECUTABLE)
    message(STATUS "cppcheck found: ${CPPCHECK_EXECUTABLE}")
    add_custom_command(
	TARGET ${PROJECT_NAME}_c
	COMMAND ${CPPCHECK_EXECUTABLE}
	  --enable=warning,style,performance,portability
	  --std=c++23
	  --template=gcc
	  --verbose
	  --force
	  --quiet
          ${sources}
	WORKING_DIRECTORY
	  ${CMAKE_CURRENT_SOURCE_DIR}
    )
  else()
    message(STATUS "cppcheck not found in $PATH. Please consider installing cppcheck for additional checks!")
  endif()

endif()

# ========= Dynamic Analyzer Checks ==========

option(ASAN OFF "Compile library and executables with LLVM Address Sanitizer")
if(ASAN)
  if(NOT TARGET asan)
    find_package(sanitizer REQUIRED COMPONENTS asan)
  endif()
  target_link_libraries(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:asan>)
endif()

option(UBSAN OFF "Compile library and executables with LLVM Undefined Behavior Sanitizer")
if(UBSAN)
  if(NOT TARGET ubsan)
    find_package(sanitizer REQUIRED COMPONENTS ubsan)
  endif()
  target_link_libraries(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:ubsan>)
endif()

option(MSAN OFF "Compile library and executables with LLVM Memory Sanitizer")
if(MSAN)
  if(NOT TARGET msan)
    find_package(sanitizer REQUIRED COMPONENTS msan)
  endif()
  target_link_libraries(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:msan>)
endif()

option(TSAN OFF "Compile library and executables with LLVM Thread Sanitizer")
if(TSAN)
  if(NOT TARGET tsan)
    find_package(sanitizer REQUIRED COMPONENTS tsan)
  endif()
  target_link_libraries(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:tsan>)
endif()
