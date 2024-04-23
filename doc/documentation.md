@page documentation API Documentation

[TOC]

The **h5** library implements various high-level C++ wrappers around their low-level C counterparts.
It is not intended as a full replacement for the HDF5 C implementation.
Instead it tries to help the user with the most common tasks like opening/closing files, creating groups/datasets
and writing/reading data to/from HDF5 files.

The following provides a detailed reference documentation grouped into logical units.

If you are looking for a specific function, class, etc., try using the search bar in the top left corner.

## h5 data model

The @ref data_model forms the backbone of the **h5** library by implementing some of the key concepts from the
[abstract HDF5 data model](https://docs.hdfgroup.org/hdf5/develop/_h5_d_m__u_g.html#subsec_data_model_abstract).
It provides concrete types for HDF5 files, groups, datasets, dataspaces and so on.

The most fundamental type in **h5** is the h5::object.
It serves either as a base class for

* @ref h5::file
* @ref h5::group

or as a type alias for

* @ref h5::dataset
* @ref h5::datatype
* @ref h5::attribute
* @ref h5::proplist
* @ref h5::attribute

For the average user, the two important types are the h5::file and h5::group.
They simplify basic tasks like creating, opening and manipulating HDF5 files, groups, subgroups, datasets and links.

## h5 types

@ref h5_types offers various functionality related to HDF5 datatypes, e.g. mapping C++ types to their corresponding
HDF5 datatypes (see h5::hdf5_type) or getting the type stored in an HDF5 dataset (see h5::get_hdf5_type).

## Read and write from/to HDF5

@ref readwrite contains all the tools to actually read or write HDF5 files.

At the highest level it provides a @ref rw_generic "generic interface" for reading/writing from/to HDF5.
Internally, the generic functions call more specialized implementations depending on what types are being passed to
the interface.

**h5** has native support for various types:

* @ref rw_scalar
* @ref rw_array
* @ref rw_map
* @ref rw_optional
* @ref rw_pair
* @ref rw_tuple
* @ref rw_variant
* @ref rw_vector

The @ref rw_arrayinterface "array interface" helps with loading and storing n-dimensional arrays.

Furthermore, the generic design of the read/write functionality makes it easily extendible to support custom user types as well.
@ref ex2 shows how to make a user defined type HDF5 serializable.

## Serialize/Deserialize

**h5** can also be used to @ref serialize an object to/from a byte buffer using the two generic functions:

* h5::serialize: Store a given object in a byte buffer.
* h5::deserialize: Restore an object from a given byte buffer.

## Utilities

@ref utilities is a collection of convenience functions, definitions and various other tools which do not fit
into any other category above but are used throughout the **h5** library.
