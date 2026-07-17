.. highlight:: bash

.. _install:

Install h5
**********

Compiling h5 from source
========================

.. note:: To guarantee reproducibility in scientific calculations we strongly recommend the use of a stable `release <https://github.com/TRIQS/h5/releases>`_.

Prerequisites
-------------

The C++ ``h5`` library requires:

#. A C++20 compatible compiler
#. CMake version 3.20 or later
#. The HDF5 library

The Python interface additionally requires Python and numpy. The Python extension module is
compiled with the help of `c2py <https://github.com/flatironinstitute/c2py>`_, which is fetched
automatically when running CMake with ``-DPythonSupport=ON``.

We do not provide explicit version requirements. Instead we refer the user to the ``h5`` CI
builds on `GitHub Actions <https://github.com/TRIQS/h5/actions>`_, where recent OS, HDF5 and
compiler versions are tested.

Installation steps
------------------

#. Download the source code of the latest stable version by cloning the ``TRIQS/h5`` repository from GitHub::

     $ git clone https://github.com/TRIQS/h5 h5.src

#. Create and move to a new directory where you will compile the code::

     $ mkdir h5.build && cd h5.build

#. In the build directory call cmake, specifying an install prefix and any additional custom CMake options (see below).
   Note that specifying ``CMAKE_INSTALL_PREFIX`` is required::

     $ cmake ../h5.src -DCMAKE_INSTALL_PREFIX=path_to_install_dir

#. Compile the code, run the tests and install the application::

     $ make
     $ make test
     $ make install

Version compatibility
---------------------

To use a particular version, go into the directory with the sources, and look at all available versions::

     $ cd h5.src && git tag

Checkout the version of the code that you want::

     $ git checkout 1.3.0

and follow steps 2 to 4 above to compile the code.

Custom CMake options
--------------------

The compilation of ``h5`` can be configured using CMake-options::

    cmake ../h5.src -DCMAKE_INSTALL_PREFIX=path_to_install_dir -DOPTION1=value1 -DOPTION2=value2 ...

+-----------------------------------------------------------------+--------------------------------------------------+
| Options                                                         | Syntax                                           |
+=================================================================+==================================================+
| Specify an installation path                                    | -DCMAKE_INSTALL_PREFIX=path_to_install_dir       |
+-----------------------------------------------------------------+--------------------------------------------------+
| Build in Debugging Mode                                         | -DCMAKE_BUILD_TYPE=Debug                         |
+-----------------------------------------------------------------+--------------------------------------------------+
| Disable testing (not recommended)                               | -DBuild_Tests=OFF                                |
+-----------------------------------------------------------------+--------------------------------------------------+
| Build the documentation                                         | -DBuild_Documentation=ON                         |
+-----------------------------------------------------------------+--------------------------------------------------+
| Disable Python support                                          | -DPythonSupport=OFF                              |
+-----------------------------------------------------------------+--------------------------------------------------+
| Build shared libraries                                          | -DBUILD_SHARED_LIBS=ON                           |
+-----------------------------------------------------------------+--------------------------------------------------+
