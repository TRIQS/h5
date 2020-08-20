file(GLOB_RECURSE sources *.cpp)
add_library(${PROJECT_NAME}_c ${sources})
add_library(${PROJECT_NAME}::${PROJECT_NAME}_c ALIAS ${PROJECT_NAME}_c)

# Enable warnings
target_link_libraries(${PROJECT_NAME}_c PRIVATE $<BUILD_INTERFACE:${PROJECT_NAME}_warnings>)

# Set C++ Standard
if(HAS_CPP20)
  target_compile_options(${PROJECT_NAME}_c PUBLIC -std=c++20)
else()
  target_compile_options(${PROJECT_NAME}_c PUBLIC -std=c++17 $<$<CXX_COMPILER_ID:GNU>:-fconcepts>)
endif()

# Configure target and compilation
set_target_properties(${PROJECT_NAME}_c PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_include_directories(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/c++>)
target_include_directories(${PROJECT_NAME}_c SYSTEM INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>)
target_compile_definitions(${PROJECT_NAME}_c PUBLIC
				H5_GIT_HASH=${PROJECT_GIT_HASH}
				$<$<CONFIG:Debug>:H5_DEBUG>
			  )

# Install library and headers
install(TARGETS ${PROJECT_NAME}_c EXPORT ${PROJECT_NAME}-targets DESTINATION lib)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

# ========= Additional Depdencies ==========

message(STATUS "-------- HDF5 detection -------------")

find_package(HDF5 REQUIRED C HL)

# Create an interface target
add_library(hdf5 INTERFACE)
add_library(h5::hdf5 ALIAS hdf5)
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
  target_link_libraries(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:asan>)
endif()
if(UBSAN)
  if(NOT TARGET ubsan)
    find_package(sanitizer REQUIRED "ubsan")
  endif()
  target_link_libraries(${PROJECT_NAME}_c PUBLIC $<BUILD_INTERFACE:ubsan>)
endif()
