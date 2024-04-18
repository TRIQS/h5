@page integration Integration in C++ projects

[TOC]

**h5** is a compiled library.

You can either
* build and install it beforehand (see @ref installation) and then link against it (see @ref find_package) or
* let CMake fetch and build it directly as part of your project (see @ref fetch).

To use **h5** in your own `C++` code, you simply have to include the relevant header files.
For example:

```cpp
#include <h5/h5.hpp>

// use h5
```


@section cmake CMake

@subsection fetch FetchContent

If you use [CMake](https://cmake.org/) to build your source code, it is recommended to fetch the **h5** directly from the
[Github repository](https://github.com/TRIQS/h5) using CMake's [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)
module:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project CXX)

# fetch from github
include (FetchContent)
FetchContent_Declare(
  h5
  GIT_REPOSITORY https://github.com/TRIQS/h5.git
  GIT_TAG        1.2.x
)
FetchContent_MakeAvailable(h5)

# declare a target and link to h5
add_executable(my_executable main.cpp)
target_link_libraries(my_executable h5::h5_c)
```

If you need to use some of the HDF5 C library features directly, you can simply link to it via `h5::hdf5`.

Note that the above will also build [goolgetest](https://github.com/google/googletest) and the unit tests for **h5**.
To disable this, you can put `set(Build_Tests OFF CACHE BOOL "" FORCE)` before fetching the content or by specifying
`-DBuild_Tests=OFF` on the command line.


@subsection find_package find_package

If you have already installed **h5** on your system by following the instructions from the @ref installation page, you can also make
use of CMake's [find_package](https://cmake.org/cmake/help/latest/command/find_package.html) command.
This has the advantage that you don't need to download anything, i.e. no internet connection is required.
Furthermore, you only need to build the library once and can use it in multiple independent projects.

Let's assume that **h5** has been installed to `path_to_install_dir`.
Then linking your project to **h5** with CMake is as easy as

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project CXX)

# find h5
find_package(h5 REQUIRED CONFIG)

# declare a target and link to h5
add_executable(my_executable main.cpp)
target_link_libraries(my_executable h5::h5_c)
```

In case, CMake cannot find the package, you might have to tell it where to look for the `h5-config.cmake` file by setting the variable
`h5_DIR` to `path_to_install_dir/lib/cmake/h5` or by sourcing the provided `h5vars.sh` before running CMake:

```console
$ source path_to_install_dir/share/h5/h5vars.sh
```


@subsection add_sub add_subdirectory

You can also integrate **h5** into your CMake project by placing the entire source tree in a subdirectory and call `add_subdirectory()`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project CXX)

# add h5 subdirectory
add_subdirectory(deps/h5)

# declare a target and link to h5
add_executable(my_executable main.cpp)
target_link_libraries(my_executable h5::h5_c)
```

Here, it is assumed that the **h5** source tree is in a subdirectory `deps/h5` relative to your `CMakeLists.txt` file.
