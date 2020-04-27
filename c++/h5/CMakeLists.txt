file(GLOB_RECURSE sources *.cpp)
add_library(h5_c STATIC ${sources})

# Enable warnings
target_link_libraries(h5_c PRIVATE project_warnings)

# Configure compilation
target_compile_options(h5_c PUBLIC -fPIC -std=c++17 $<$<CXX_COMPILER_ID:GNU>:-fconcepts>)
target_include_directories(h5_c PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/c++>)
target_compile_definitions(h5_c PUBLIC
  				H5_GIT_HASH=${PROJECT_GIT_HASH}
				$<$<CONFIG:Debug>:H5_DEBUG>
			  )

# Install library and headers
install(TARGETS h5_c EXPORT h5-targets DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")


# ========= Additional Depdencies ==========

message(STATUS "-------- HDF5 detection -------------")

find_package(HDF5 REQUIRED C HL)

# Create an interface target
add_library(hdf5 INTERFACE)
target_include_directories(hdf5 SYSTEM INTERFACE ${HDF5_INCLUDE_DIRS})
target_link_libraries(hdf5 INTERFACE "${HDF5_LIBRARIES}" ${HDF5_HL_LIBRARIES})
target_compile_options(hdf5 INTERFACE ${HDF5_DEFINITIONS})
if(HDF5_VERSION VERSION_GREATER 1.10)
  target_compile_definitions(hdf5 INTERFACE H5_USE_110_API)
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
    set_target_properties(h5_c PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
  else()
    message(STATUS "clang-tidy not found in $PATH. Please consider installing clang-tidy for additional checks!")
  endif()

  # Run cppcheck if found
  if(CPPCHECK_EXECUTABLE)
    message(STATUS "cppcheck found: ${CPPCHECK_EXECUTABLE}")
    add_custom_command(
	TARGET h5_c
	COMMAND ${CPPCHECK_EXECUTABLE}
	  --enable=warning,style,performance,portability
	  --std=c++17
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
option(UBSAN OFF "Compile library and executables with LLVM Undefined Behavior Sanitizer")

if(ASAN)
  if(NOT TARGET asan)
    find_package(sanitizer REQUIRED "asan")
  endif()
  target_link_libraries(h5_c PUBLIC asan)
  install(targets asan export h5-targets)
endif()
if(UBSAN)
  if(NOT TARGET ubsan)
    find_package(sanitizer REQUIRED "ubsan")
  endif()
  target_link_libraries(h5_c PUBLIC ubsan)
  install(TARGETS ubsan EXPORT h5-targets)
endif()
