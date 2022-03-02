(changelog)=

# Changelog

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
