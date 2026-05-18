@page installation Installation

[TOC]

**h5** supports the usual installation procedure using CMake.

If you want to skip the installation step, you can go directly to @ref integration to see how you can integrate
**h5** into your own C++ project by using CMake's @ref fetch.

> **Note:** To guarantee reproducibility in scientific calculations, we strongly recommend the use of a stable
> [release version](https://github.com/TRIQS/h5/releases).


@section dependencies Dependencies

The dependencies of the C++ **h5** library are as follows:

* C++20 compatible compiler 
* CMake version 3.20
* HDF5 library

The Python interface requires additional dependencies:

* Python
* numpy

The Python extension module is compiled with the help of [c2py](https://github.com/flatironinstitute/c2py) which is
fetched automatically when running CMake with ``PythonSupport=ON``.

We do not provide explicit version requirements.
Instead, we refer the user to the **h5** CI builds on [GitHub Actions](https://github.com/TRIQS/h5/actions), where 
recent OS, HDF5 and compiler versions are tested.


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
$ git checkout 1.3.0
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
