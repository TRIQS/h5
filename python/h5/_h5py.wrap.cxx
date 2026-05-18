
// C.f. https://numpy.org/doc/1.21/reference/c-api/array.html#importing-the-api
#define PY_ARRAY_UNIQUE_SYMBOL _cpp2py_ARRAY_API
#ifndef CLAIR_C2PY_WRAP_GEN
#ifdef __clang__
// #pragma clang diagnostic ignored "-W#warnings"
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#pragma GCC diagnostic ignored "-Wcpp"
#endif

#define C2PY_VERSION_MAJOR 0
#define C2PY_VERSION_MINOR 1

#include <c2py/c2py.hpp>

using c2py::operator""_a;

// ==================== enums =====================

// ==================== module classes =====================

// --------- class _c2py_cls_0 -----------
using _c2py_cls_0 = h5::file;
template <>
constexpr bool c2py::is_wrapped<_c2py_cls_0> = true;
template <>
inline constexpr auto c2py::tp_name<_c2py_cls_0> = "h5._h5py.File";
static const auto _c2py_init_0                   = c2py::dispatcher_c_kw_t{
   c2py::c_constructor<_c2py_cls_0>(), c2py::c_constructor<_c2py_cls_0, const char *, char>("name", "mode"),
   c2py::c_constructor<_c2py_cls_0, const std::string &, char>("name", "mode"), c2py::c_constructor<_c2py_cls_0, const std::span<std::byte> &>("buf"),
   c2py::c_constructor<_c2py_cls_0, const std::vector<std::byte> &>("buf")};
template <>
constexpr initproc c2py::tp_init<_c2py_cls_0> = c2py::pyfkw_constructor<_c2py_init_0>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_0> =
   _c2py_init_0.doc(R"DOC(
[1] Default constructor creates a buffered memory file.

It modifies the file access property list to use the `H5FD_CORE` driver. It can be used for serializing 
and deserializing data (see h5::serialize and h5::deserialize).

------

[2] Constructor to open an existing file or to create a new file on disk.

The file is opened in the specified mode. The following modes are available:

- 'r': Open an existing file in read only mode (calls `H5Fopen` with `H5F_ACC_RDONLY`).
- 'w': Create a new file or overwrite an existing file in read-write mode (calls `H5Fcreate` with 
`H5F_ACC_TRUNC`).
- 'a': Create a new file or append to an existing file in read-write mode (calls `H5Fcreate` with  `H5F_ACC_EXCL`
or `H5Fopen` with `H5F_ACC_RDWR` in case the file already exists).
- 'e': Create a new file if the file does not already exists, otherwise throw an exception (calls `H5Fcreate` 
with `H5F_ACC_EXCL`)

------

[3] Constructor to open an existing file or to create a new file on disk.

See file::file(const char*, char) for a more detailed description.

------

[4, 5] Constructor to create a buffered memory file from a byte buffer.

------

Parameters
----------
name : {par_0}
   Name of the file.
mode : {par_1}
   Mode in which to open the file.
buf : {par_2}
   Byte buffer.
)DOC",
                    {{c2py::python_typename<const char *>()},
                     {c2py::python_typename<char>()},
                     {c2py::python_typename<const std::span<std::byte> &>(), c2py::python_typename<const std::vector<std::byte> &>()}});
// as_buffer
static auto const _c2py_fun_0 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self) -> decltype(auto) { return self.as_buffer(); }, "self")};

// close
static auto const _c2py_fun_1 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 &self) -> decltype(auto) { return self.close(); }, "self")};

// flush
static auto const _c2py_fun_2 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 &self) -> decltype(auto) { return self.flush(); }, "self")};

// get_ref_count
static auto const _c2py_fun_3 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self) -> decltype(auto) { return self.get_ref_count(); }, "self")};

// is_valid
static auto const _c2py_fun_4 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self) -> decltype(auto) { return self.is_valid(); }, "self")};

// name
static auto const _c2py_fun_5 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_0 const &self) -> decltype(auto) { return self.name(); }, "self")};

static const auto _c2py_doc_0 = _c2py_fun_0.doc(R"DOC(
Get a copy of the associated byte buffer.
)DOC");
static const auto _c2py_doc_1 = _c2py_fun_1.doc(R"DOC(
Release the HDF5 handle by decreasing the reference count and by setting the object's ID to zero.
)DOC");
static const auto _c2py_doc_2 = _c2py_fun_2.doc(R"DOC(
Flush the file by calling `H5Fflush`.
)DOC");
static const auto _c2py_doc_3 = _c2py_fun_3.doc(R"DOC(
Get the current reference count.
)DOC");
static const auto _c2py_doc_4 = _c2py_fun_4.doc(R"DOC(
Ensure that the wrapped HDF5 ID is valid (by calling `H5Iis_valid`).
)DOC");
static const auto _c2py_doc_5 = _c2py_fun_5.doc(R"DOC(
Get the name of the file.
)DOC");

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_0>[] = {
   {"as_buffer", (PyCFunction)c2py::pyfkw<_c2py_fun_0>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_0.c_str()},
   {"close", (PyCFunction)c2py::pyfkw<_c2py_fun_1>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_1.c_str()},
   {"flush", (PyCFunction)c2py::pyfkw<_c2py_fun_2>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_2.c_str()},
   {"get_ref_count", (PyCFunction)c2py::pyfkw<_c2py_fun_3>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_3.c_str()},
   {"is_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_4>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_4.c_str()},
   {"name", (PyCFunction)c2py::pyfkw<_c2py_fun_5>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_5.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <>
const std::string c2py::tp_doc<_c2py_cls_0> = R"DOC(A handle to an HDF5 file.

This class inherits from the general h5::object class. It simply wraps the HDF5 functions to open an 
existing file or to create a new file.

An h5::file is automatically closed when it goes out of scope, i.e. its reference count is decreased.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_0>;
// --------- class _c2py_cls_1 -----------
using _c2py_cls_1 = h5::group;
template <>
constexpr bool c2py::is_wrapped<_c2py_cls_1> = true;
template <>
inline constexpr auto c2py::tp_name<_c2py_cls_1> = "h5._h5py.Group";
static const auto _c2py_init_1 = c2py::dispatcher_c_kw_t{c2py::c_constructor<_c2py_cls_1>(), c2py::c_constructor<_c2py_cls_1, h5::file>("f")};
template <>
constexpr initproc c2py::tp_init<_c2py_cls_1> = c2py::pyfkw_constructor<_c2py_init_1>;
template <>
const std::string c2py::tp_ctor_doc<_c2py_cls_1> = _c2py_init_1.doc(R"DOC(
[1] Default constructor (only necessary for the Python interface).

------

[2] Constructor to open the root ("/") group in the given file.

------

Parameters
----------
f : {par_0}
   h5::file.
)DOC",
                                                                    {{c2py::python_typename<h5::file>()}});
// close
static auto const _c2py_fun_6 = c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 &self) -> decltype(auto) { return self.close(); }, "self")};

// create_group
static auto const _c2py_fun_7 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, const std::string &key, bool delete_if_exists) -> decltype(auto) { return self.create_group(key, delete_if_exists); },
   "self", "key", "delete_if_exists"_a = true)};

// create_softlink
static auto const _c2py_fun_8 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, const std::string &target_key, const std::string &key,
                    bool delete_if_exists) -> decltype(auto) { return self.create_softlink(target_key, key, delete_if_exists); },
                 "self", "target_key", "key", "delete_if_exists"_a = true)};

// get_all_dataset_names
static auto const _c2py_fun_9 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_all_dataset_names(); }, "self")};

// get_all_subgroup_dataset_names
static auto const _c2py_fun_10 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_all_subgroup_dataset_names(); }, "self")};

// get_all_subgroup_names
static auto const _c2py_fun_11 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_all_subgroup_names(); }, "self")};

// get_file
static auto const _c2py_fun_12 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_file(); }, "self")};

// get_ref_count
static auto const _c2py_fun_13 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.get_ref_count(); }, "self")};

// has_dataset
static auto const _c2py_fun_14 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, const std::string &key) -> decltype(auto) { return self.has_dataset(key); }, "self", "key")};

// has_key
static auto const _c2py_fun_15 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, const std::string &key) -> decltype(auto) { return self.has_key(key); }, "self", "key")};

// has_subgroup
static auto const _c2py_fun_16 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, const std::string &key) -> decltype(auto) { return self.has_subgroup(key); }, "self", "key")};

// is_valid
static auto const _c2py_fun_17 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.is_valid(); }, "self")};

// keys
static auto const _c2py_fun_18 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.keys(); }, "self")};

// name
static auto const _c2py_fun_19 =
   c2py::dispatcher_f_kw_t{c2py::cmethod([](_c2py_cls_1 const &self) -> decltype(auto) { return self.name(); }, "self")};

// open_group
static auto const _c2py_fun_20 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, const std::string &key) -> decltype(auto) { return self.open_group(key); }, "self", "key")};

// read_attribute
static auto const _c2py_fun_21 = c2py::dispatcher_f_kw_t{
   c2py::cmethod([](_c2py_cls_1 const &self, const std::string &name) -> decltype(auto) { return self.read_attribute(name); }, "self", "name")};

// read_hdf5_format_from_key
static auto const _c2py_fun_22 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, const std::string &key) -> decltype(auto) { return self.read_hdf5_format_from_key(key); }, "self", "key")};

// unlink
static auto const _c2py_fun_23 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, const std::string &key, bool error_if_absent) -> decltype(auto) { return self.unlink(key, error_if_absent); }, "self",
   "key", "error_if_absent"_a = false)};

// write_attribute
static auto const _c2py_fun_24 = c2py::dispatcher_f_kw_t{c2py::cmethod(
   [](_c2py_cls_1 const &self, const std::string &name, const std::string &s) -> decltype(auto) { return self.write_attribute(name, s); }, "self",
   "name", "s")};

static const auto _c2py_doc_6 = _c2py_fun_6.doc(R"DOC(
Release the HDF5 handle by decreasing the reference count and by setting the object's ID to zero.
)DOC");
static const auto _c2py_doc_7 =
   _c2py_fun_7.doc(R"DOC(
Create a subgroup with the given key in the group.

If a subgroup with the given key already exists, it is unlinked first if `delete_if_exists == true`. If 
the given key is empty, a handle to the current group is returned. Throws an exception if the subgroup fails to 
be created.

Parameters
----------
key : {par_0}
   Name of the subgroup to be created.
delete_if_exists : {par_1}
   If true, unlink first an existing subgroup with the same name.

Returns
-------
{ret_0}
   A handle to the created subgroup.
)DOC",
                   {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<bool>()}}, {c2py::python_typename<h5::group>()});
static const auto _c2py_doc_8 =
   _c2py_fun_8.doc(R"DOC(
Create a softlink with the given key to a target with a given target key in this group.

Does nothing if the key or target key is empty. If `delete_if_exists == true`, it first unlinks an 
existing link with the same name. Throws an exception if the target does not exist, if a link with the given key 
already exists and `delete_if_exists == false`, or if the softlink fails to be created.

Parameters
----------
target_key : {par_0}
   Name of target.
key : {par_1}
   Name of the softlink to be created.
delete_if_exists : {par_2}
   If true, unlink first an existing key with the same name.
)DOC",
                   {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<const std::string &>()}, {c2py::python_typename<bool>()}});
static const auto _c2py_doc_9  = _c2py_fun_9.doc(R"DOC(
Get all the names of the datasets in the current group.

Returns
-------
{ret_0}
   A vector with the names of all the datasets.
)DOC",
                                                 {}, {c2py::python_typename<std::vector<std::string>>()});
static const auto _c2py_doc_10 = _c2py_fun_10.doc(R"DOC(
Get all the names of the subgroups and datasets in the current group.

Returns
-------
{ret_0}
   A vector with the names of all the subgroups and datasets.
)DOC",
                                                  {}, {c2py::python_typename<std::vector<std::string>>()});
static const auto _c2py_doc_11 = _c2py_fun_11.doc(R"DOC(
Get all the names of the subgroups in the current group.

Returns
-------
{ret_0}
   A vector with the names of all the subgroups.
)DOC",
                                                  {}, {c2py::python_typename<std::vector<std::string>>()});
static const auto _c2py_doc_12 = _c2py_fun_12.doc(R"DOC(
Get the parent file to which the group belongs.
)DOC");
static const auto _c2py_doc_13 = _c2py_fun_13.doc(R"DOC(
Get the current reference count.
)DOC");
static const auto _c2py_doc_14 = _c2py_fun_14.doc(R"DOC(
Check if a dataset with the given key exists in the group and is accessible.

Parameters
----------
key : {par_0}
   Name of the dataset.

Returns
-------
{ret_0}
   True if the dataset exists, false otherwise.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_15 = _c2py_fun_15.doc(R"DOC(
Check if a link with the given key exists in the group.

Parameters
----------
key : {par_0}
   Name of the link.

Returns
-------
{ret_0}
   True if the link exists, false otherwise.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_16 = _c2py_fun_16.doc(R"DOC(
Check if a subgroup with the given key exists in the group and is accessible.

Parameters
----------
key : {par_0}
   Name of the subgroup.

Returns
-------
{ret_0}
   True if the subgroup exists and can be accessed, false otherwise.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<bool>()});
static const auto _c2py_doc_17 = _c2py_fun_17.doc(R"DOC(
Ensure that the wrapped HDF5 ID is valid (by calling `H5Iis_valid`).
)DOC");
static const auto _c2py_doc_18 = _c2py_fun_18.doc(R"DOC(
Get all the names of the subgroups and datasets in the current group.

This is the same as get_all_subgroup_dataset_names() (used in the Python interface).

Returns
-------
{ret_0}
   A vector with the names of all the subgroups and datasets.
)DOC",
                                                  {}, {c2py::python_typename<std::vector<std::string>>()});
static const auto _c2py_doc_19 = _c2py_fun_19.doc(R"DOC(
Get the name of the group.
)DOC");
static const auto _c2py_doc_20 = _c2py_fun_20.doc(R"DOC(
Open a subgroup with the given key in the group.

If the given key is empty, a handle to the current group is returned. Throws an exception if the 
subgroup fails to be opened.

Parameters
----------
key : {par_0}
   Name of the subgroup.

Returns
-------
{ret_0}
   A handle to the opened subgroup.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<h5::group>()});
static const auto _c2py_doc_21 = _c2py_fun_21.doc(R"DOC(
Read a string from an HDF5 attribute with the given name.

It simply calls h5::h5_read_attribute with `this` group and the given name.

Parameters
----------
name : {par_0}
   Name of the attribute.

Returns
-------
{ret_0}
   `std::string` containing the attribute.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<std::string>()});
static const auto _c2py_doc_22 = _c2py_fun_22.doc(R"DOC(
Read an `hdf5_format` tag from an HDF5 attribute with the name 'Format'.

It simply calls h5::read_hdf5_format_from_key with `this` group and the given key.

Parameters
----------
key : {par_0}
   Name of the object from which the attribute is read.

Returns
-------
{ret_0}
   `std::string` containing the `hdf5_format` tag.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}}, {c2py::python_typename<std::string>()});
static const auto _c2py_doc_23 = _c2py_fun_23.doc(R"DOC(
Remove a link with the given key from the group.

It simply calls `H5Ldelete` to delete the link. If the given link does not exist, it throws an exception 
if `error_if_absent == true`, otherwise it does nothing.

Parameters
----------
key : {par_0}
   Name of the link to be removed.
error_if_absent : {par_1}
   If true, throws an exception if the key is not the name of a link in the group.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<bool>()}});
static const auto _c2py_doc_24 = _c2py_fun_24.doc(R"DOC(
Write a `std::string` HDF5 attribute with the given name to the group.

It simply calls h5::h5_write_attribute with `this` group and the given name.

Parameters
----------
name : {par_0}
   Name of the attribute.
s : {par_1}
   `std::string` to be written.
)DOC",
                                                  {{c2py::python_typename<const std::string &>()}, {c2py::python_typename<const std::string &>()}});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<_c2py_cls_1>[] = {
   {"close", (PyCFunction)c2py::pyfkw<_c2py_fun_6>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_6.c_str()},
   {"create_group", (PyCFunction)c2py::pyfkw<_c2py_fun_7>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_7.c_str()},
   {"create_softlink", (PyCFunction)c2py::pyfkw<_c2py_fun_8>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_8.c_str()},
   {"get_all_dataset_names", (PyCFunction)c2py::pyfkw<_c2py_fun_9>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_9.c_str()},
   {"get_all_subgroup_dataset_names", (PyCFunction)c2py::pyfkw<_c2py_fun_10>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_10.c_str()},
   {"get_all_subgroup_names", (PyCFunction)c2py::pyfkw<_c2py_fun_11>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_11.c_str()},
   {"get_file", (PyCFunction)c2py::pyfkw<_c2py_fun_12>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_12.c_str()},
   {"get_ref_count", (PyCFunction)c2py::pyfkw<_c2py_fun_13>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_13.c_str()},
   {"has_dataset", (PyCFunction)c2py::pyfkw<_c2py_fun_14>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_14.c_str()},
   {"has_key", (PyCFunction)c2py::pyfkw<_c2py_fun_15>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_15.c_str()},
   {"has_subgroup", (PyCFunction)c2py::pyfkw<_c2py_fun_16>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_16.c_str()},
   {"is_valid", (PyCFunction)c2py::pyfkw<_c2py_fun_17>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_17.c_str()},
   {"keys", (PyCFunction)c2py::pyfkw<_c2py_fun_18>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_18.c_str()},
   {"name", (PyCFunction)c2py::pyfkw<_c2py_fun_19>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_19.c_str()},
   {"open_group", (PyCFunction)c2py::pyfkw<_c2py_fun_20>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_20.c_str()},
   {"read_attribute", (PyCFunction)c2py::pyfkw<_c2py_fun_21>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_21.c_str()},
   {"read_hdf5_format_from_key", (PyCFunction)c2py::pyfkw<_c2py_fun_22>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_22.c_str()},
   {"unlink", (PyCFunction)c2py::pyfkw<_c2py_fun_23>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_23.c_str()},
   {"write_attribute", (PyCFunction)c2py::pyfkw<_c2py_fun_24>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_24.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

template <>
const std::string c2py::tp_doc<_c2py_cls_1> = R"DOC(A handle to an HDF5 group.

This class inherits from the general h5::object class. Each group stores the parent h5::file to which it 
belongs.

It provides various methods to simplify the creation of new and opening of existing groups, subgroups, datasets and 
softlinks within the current group.)DOC"
   + std::string{"\n\n----------\n\n"} + c2py::tp_ctor_doc<_c2py_cls_1>;

// ==================== module functions ====================

// h5_read_bare
static auto const _c2py_fun_25 =
   c2py::dispatcher_f_kw_t{c2py::cfun([](h5::group g, const std::string &name) { return h5::h5_read_bare(g, name); }, "g", "name")};

// h5_write_bare
static auto const _c2py_fun_26 = c2py::dispatcher_f_kw_t{
   c2py::cfun([](h5::group g, const std::string &name, _object *ob) { return h5::h5_write_bare(g, name, ob); }, "g", "name", "ob")};

static const auto _c2py_doc_25 =
   _c2py_fun_25.doc(R"DOC(
Read a dataset from an HDF5 group and return it as a Python object.

The Python type is determined from the dataset's HDF5 type and its rank:
- Rank 0:
  - `H5T_FLOAT` :math:`\rightarrow` Python float
  - `H5T_INTEGER` :math:`\rightarrow` Python int
  - `H5T_STRING` :math:`\rightarrow` Python str
- Rank 1 and h5::array_interface::dataset_info has complex attribute :math:`\rightarrow` Python complex
- Rank 0, 1 or 2 and `H5T_STRING` :math:`\rightarrow` Python str or list of (list of) str
- Otherwise :math:`\rightarrow` numpy ndarray

Parameters
----------
g : {par_0}
   h5::group containing the dataset.
name : {par_1}
   Name of the dataset from which the object is read.

Returns
-------
{ret_0}
   `c2py::pyref` wrapping the Python object (new reference).
)DOC",
                    {{c2py::python_typename<h5::group>()}, {c2py::python_typename<const std::string &>()}}, {c2py::python_typename<c2py::pyref>()});
static const auto _c2py_doc_26 =
   _c2py_fun_26.doc(R"DOC(
Write a Python object to an HDF5 group.

Only scalars, strings and numpy arrays are supported. They are first converted to a corresponding C++ type
and then written to HDF5 using the functionality provided by **h5**:
- numpy ndarray :math:`\rightarrow` h5::array_interface::array_view
- numpy scalar :math:`\rightarrow` 0-dimensional numpy array :math:`\rightarrow` h5::array_interface::array_view
- Python float :math:`\rightarrow` `double`
- Python bool :math:`\rightarrow` `bool`
- Python int :math:`\rightarrow` `long`
- Python str :math:`\rightarrow` `const char *`
- Python complex :math:`\rightarrow` `std::complex<double>`

Parameters
----------
g : {par_0}
   h5::group in which the dataset is created.
name : {par_1}
   Name of the dataset to which the object is written.
ob : {par_2}
   Python object to be written.
)DOC",
                    {{c2py::python_typename<h5::group>()}, {c2py::python_typename<const std::string &>()}, {c2py::python_typename<_object *>()}});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"h5_read_bare", (PyCFunction)c2py::pyfkw<_c2py_fun_25>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_25.c_str()},
   {"h5_write_bare", (PyCFunction)c2py::pyfkw<_c2py_fun_26>, METH_VARARGS | METH_KEYWORDS, _c2py_doc_26.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "_h5py",                                                    /* name of module */
                                        R"RAWDOC(Python bindings for the TRIQS/h5 library.)RAWDOC", /* module documentation, may be NULL */
                                        -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit__h5py() {

  if (not c2py::check_python_version("_h5py")) return NULL;

  // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_0>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<_c2py_cls_1>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
#define _add_type(T, N) c2py::add_type_object_to_main<T>(N, m, conv_table)
  _add_type(_c2py_cls_0, "File");
  _add_type(_c2py_cls_1, "Group");
#undef _add_type

  return m;
}
#endif
// CLAIR_WRAP_GEN
