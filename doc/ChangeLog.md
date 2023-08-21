(changelog)=

# Changelog

## Version 1.2.0

This is version 1.2.0 of h5, a high-level C++ interface to the hdf5 library.

We thank all contributors: Thomas Hahn, Alexander Hampel, Olivier Parcollet, Dylan Simon, Nils Wentzell

Find below an itemized list of changes in this release.

### General
* Remove redundant std_addons/complex.hpp header
* Rename h5::h5_try_read -> h5::try_read
* Add concept h5::Storable
* Fix issue in previous commit: Use H5_ASSERT over ASSERT macro
* Add function write_slice to enable updating part of an existing dataset
* Allow to set hyperslab for filespace in h5_read
* Extend vector test to cover complex numbers
* Use warnings module to issue python warnings
* clang-format all source files
* Remove warnings
* General cleanup

### cmake
* Add compiler warnings for IntelLLVM
* Update Findsanitizer.cmake to include TSAN and MSAN
* Do not build documentation as subproject
* Fix FindHDF5 logic for cmake versions >=3.23
* Fix h5::hdf5 interface target for cmake versions 3.20+

### jenkins
* Update packaging Dockerfiles

### fixes
* Fix hsize_t for hdf5 versions >=1.13
* Loosen type-check of hsize_t to restore hdf5 1.13 compatibility Fix #11
* Fix #10: Do not exceed maximum chunk size in h5::array_interface::write
* Fix signature of generic h5::read_attribute and h5::write_attribute
* Allow reading vector<string> when stored as subgroup with numbered keys
* np.int / np.float / np.complex removed


## Version 1.1.0

This is version 1.1.0 of h5, a high-level C++ interface to the hdf5 library.

We thank all contributors: Daniel Bauernfeind, Alexander Hampel, Henri Menke, Dylan Simon, Nils Wentzell

Find below an itemized list of changes in this release.

### General
* Allow creation of softlinks
* Fix logic error in test/python/archive.py
* Make test on byte buffer less stringent, compare only archive contents for memory files
* Minor correction in error msg when constructing HDFArchiveGroup as as new memory file
* Add function as_bytes to HDFArchive class and extend the wrapped File class as necessary
* Allow creation HDFArchive as a memory-file only
* Consistently use std::byte instead of char for memory files
* Merge memory file into file class, expose byte constructor to Python + tests
* Allow type conversions in h5_read, but issue warning FIX #7
* Allowed custom key class in reading/writing of a map. (#5)
* Cleanup unnecessary hdf5.h include in h5_complex test
* Minor correction in h5_read doc for array and vector
* Consistently use header guards over pragma once

### doc
* Add link to reference doc to README.md
* Minor doc cleanups for doxygen generation, add Doxyfile and update .gitignore

### cmake
* Bump Version number to 1.1.0
* Set CXX standard using target_compile_features
* Bump version requirement to 3.13.5
* Provide a namespaced alias for the python modules


## Version 1.0.0

h5 is a high-level C++ interface to the hdf5 library, which
* Provides write and read functions for various STL container types
* Provides a generic `array_interface` to store and load multi-dimensional arrays
* Takes an RAII approach to manage the reference counting.
* Comes with Python bindings.

This is the initial release for this project.
