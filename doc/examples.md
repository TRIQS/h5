@page examples Examples

[TOC]

- @ref ex1
- @ref ex2
- @ref ex3

@section compiling Compiling the examples

All examples have been compiled on a MacBook Pro with an Apple M2 Max chip and [HDF5](https://www.hdfgroup.org/solutions/hdf5/) 1.14.3
installed via [homebrew](https://brew.sh/).
We further used clang 17.0.6 together with cmake 3.29.1.

Assuming that the actual example code is in a file `main.cpp`, the following generic `CMakeLists.txt` should work for all examples:

```cmake
cmake_minimum_required(VERSION 3.20)
project(example CXX)

# set required standard
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# fetch h5 from github
set(Build_Tests OFF CACHE BOOL "" FORCE)
include (FetchContent)
FetchContent_Declare(
  h5
  GIT_REPOSITORY https://github.com/TRIQS/h5.git
  GIT_TAG        1.2.x
)
FetchContent_MakeAvailable(h5)

# build the example
add_executable(ex main.cpp)
target_link_libraries(ex h5::h5_c h5::hdf5)
```
