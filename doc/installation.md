@page installation Installation

[TOC]

**h5** supports the usual installation procedure using CMake.

If you want to skip the installation step, you can go directly to @ref integration to see how you can integrate
**h5** into your own C++ project by using CMake's @ref fetch.

> **Note:** To guarantee reproducibility in scientific calculations, we strongly recommend the use of a stable
> [release version](https://github.com/TRIQS/h5/releases).


@section dependencies Dependencies

The dependencies of the C++ **h5** library are as follows:

* gcc version 12 or later OR clang version 15 or later OR IntelLLVM (icx) 2023.1.0 or later
* CMake version 3.20 or later (for installation or integration into an existing project via CMake)
* HDF5 library version 1.8.2 or later

For the Python interface, additional dependencies are required:

* Python version 3.6 or later
* numpy version 1.11.0 or later
* mako version 0.9.1 or later
* scipy (version 1.11.3 is tested but older/newer versions should work as well)

The Python interface is built with [TRIQS/cpp2py](https://github.com/TRIQS/cpp2py).
Please refer to the [GitHub repository](https://github.com/TRIQS/cpp2py) for further information.


@section install_steps Installation steps

1. Download the source code of the latest stable version by cloning the [TRIQS/h5](https://github.com/triqs/h5)
repository from GitHub:

    ```console
    $ git clone https://github.com/TRIQS/h5 h5.src
    ```

2. Create and move to a new directory where you will compile the code:

    ```console
     $ mkdir h5.build && cd h5.build
    ```

3. In the build directory, call cmake including any additional custom CMake options (see below):

    ```console
    $ cmake ../h5.src -DCMAKE_INSTALL_PREFIX=path_to_install_dir
    ```

    Note that it is required to specify ``CMAKE_INSTALL_PREFIX``, otherwise CMake will stop with an error.

4. Compile the code, run the tests and install the application:

    ```console
    $ make -j N
    $ make test
    $ make install
    ```

    Replace `N` with the number of cores you want to use to build the library.


@section versions Versions

To choose a particular version, go into the directory with the sources, and look at all available versions:

```console
$ cd h5.src && git tag
```

Checkout the version of the code that you want:

```console
$ git checkout 1.2.0
```

and follow steps 2 to 4 above to compile the code.


@section cmake_options Custom CMake options

The compilation of **h5** can be configured by calling cmake with additional command line options

```console
$ cmake ../h5.src -DCMAKE_INSTALL_PREFIX=path_to_install_dir -DOPTION1=value1 -DOPTION2=value2 ...
```

The following options are available:

| Options                                 | Syntax                                            |
|-----------------------------------------|---------------------------------------------------|
| Specify an installation path            | ``-DCMAKE_INSTALL_PREFIX=path_to_install_dir``    |
| Build in Debugging Mode                 | ``-DCMAKE_BUILD_TYPE=Debug``                      |
| Disable testing (not recommended)       | ``-DBuild_Tests=OFF``                             |
| Build the documentation                 | ``-DBuild_Documentation=ON``                      |
| Disable Python support                  | ``-DPythonSupport=OFF``                           |
| Build shared libraries                  | ``-DBUILD_SHARED_LIBS=ON``                        |
