
// C.f. https://numpy.org/doc/1.21/reference/c-api/array.html#importing-the-api
#define PY_ARRAY_UNIQUE_SYMBOL _cpp2py_ARRAY_API
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

#include "c2py/c2py.hpp"
#include "_h5py.cpp"

using c2py::operator"" _a;

// ==================== Wrapped classes =====================

#ifndef C2PY_HXX_DECLARATION__h5py_GUARDS
#define C2PY_HXX_DECLARATION__h5py_GUARDS
template <> inline static constexpr bool c2py::is_wrapped<h5::file>  = true;
template <> inline static constexpr bool c2py::is_wrapped<h5::group> = true;
#endif

// ==================== enums =====================

// ==================== module classes =====================

template <> inline const std::string c2py::cpp_name<h5::file> = "h5::file";
template <> inline constexpr auto c2py::tp_name<h5::file>     = "_h5py._h5py.File";
template <>
inline constexpr const char *c2py::tp_doc<h5::file> = R"DOC(   A little handler for the HDF5 file
   
   The class is basically a pointer to the file.)DOC";

static auto init_0 =
   c2py::dispatcher_c_kw_t{c2py::c_constructor<h5::file>(), c2py::c_constructor<h5::file, const char *, char>("name", "mode"), c2py::c_constructor<h5::file, const std::string &, char>("name", "mode"),
                           c2py::c_constructor<h5::file, const std::span<std::byte> &>("buf"), c2py::c_constructor<h5::file, const std::vector<std::byte> &>("buf")};
template <> constexpr initproc c2py::tp_init<h5::file> = c2py::pyfkw_constructor<init_0>;
// as_buffer
static auto const fun_0 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::file::as_buffer))};

// close
static auto const fun_1 = c2py::dispatcher_f_kw_t{c2py::cfun_B<h5::file>(c2py::castm<>(&h5::file::close))};

// flush
static auto const fun_2 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castm<>(&h5::file::flush))};

// get_ref_count
static auto const fun_3 = c2py::dispatcher_f_kw_t{c2py::cfun_B<h5::file>(c2py::castmc<>(&h5::file::get_ref_count))};

// is_valid
static auto const fun_4 = c2py::dispatcher_f_kw_t{c2py::cfun_B<h5::file>(c2py::castmc<>(&h5::file::is_valid))};

// name
static auto const fun_5   = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::file::name))};
static const auto doc_d_0 = fun_0.doc({R"DOC(   Get a copy of the associated byte buffer)DOC"});
static const auto doc_d_1 = fun_1.doc({R"DOC(   Release the HDF5 handle and reset the object to default state (id =0).)DOC"});
static const auto doc_d_2 = fun_2.doc({R"DOC(   Flush the file)DOC"});
static const auto doc_d_3 = fun_3.doc({R"DOC(   Get the current number of references)DOC"});
static const auto doc_d_4 = fun_4.doc({R"DOC(   Ensure the id is valid (by H5Iis_valid).)DOC"});
static const auto doc_d_5 = fun_5.doc({R"DOC(   Name of the file)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<h5::file>[] = {
   {"as_buffer", (PyCFunction)c2py::pyfkw<fun_0>, METH_VARARGS | METH_KEYWORDS, doc_d_0.c_str()},
   {"close", (PyCFunction)c2py::pyfkw<fun_1>, METH_VARARGS | METH_KEYWORDS, doc_d_1.c_str()},
   {"flush", (PyCFunction)c2py::pyfkw<fun_2>, METH_VARARGS | METH_KEYWORDS, doc_d_2.c_str()},
   {"get_ref_count", (PyCFunction)c2py::pyfkw<fun_3>, METH_VARARGS | METH_KEYWORDS, doc_d_3.c_str()},
   {"is_valid", (PyCFunction)c2py::pyfkw<fun_4>, METH_VARARGS | METH_KEYWORDS, doc_d_4.c_str()},
   {"name", (PyCFunction)c2py::pyfkw<fun_5>, METH_VARARGS | METH_KEYWORDS, doc_d_5.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<h5::file>[] = {

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

template <> inline const std::string c2py::cpp_name<h5::group>   = "h5::group";
template <> inline constexpr auto c2py::tp_name<h5::group>       = "_h5py._h5py.Group";
template <> inline constexpr const char *c2py::tp_doc<h5::group> = R"DOC(   HDF5 group)DOC";

static auto init_1                                      = c2py::dispatcher_c_kw_t{c2py::c_constructor<h5::group>(), c2py::c_constructor<h5::group, h5::file>("f")};
template <> constexpr initproc c2py::tp_init<h5::group> = c2py::pyfkw_constructor<init_1>;
// close
static auto const fun_6 = c2py::dispatcher_f_kw_t{c2py::cfun_B<h5::group>(c2py::castm<>(&h5::group::close))};

// create_group
static auto const fun_7 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &, bool>(&h5::group::create_group), "key", "delete_if_exists"_a = true)};

// create_softlink
static auto const fun_8 =
   c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &, const std::string &, bool>(&h5::group::create_softlink), "target_key", "key", "delete_if_exists"_a = true)};

// get_all_dataset_names
static auto const fun_9 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::group::get_all_dataset_names))};

// get_all_subgroup_dataset_names
static auto const fun_10 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::group::get_all_subgroup_dataset_names))};

// get_all_subgroup_names
static auto const fun_11 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::group::get_all_subgroup_names))};

// get_file
static auto const fun_12 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::group::get_file))};

// get_ref_count
static auto const fun_13 = c2py::dispatcher_f_kw_t{c2py::cfun_B<h5::group>(c2py::castmc<>(&h5::group::get_ref_count))};

// has_dataset
static auto const fun_14 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &>(&h5::group::has_dataset), "key")};

// has_key
static auto const fun_15 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &>(&h5::group::has_key), "key")};

// has_subgroup
static auto const fun_16 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &>(&h5::group::has_subgroup), "key")};

// is_valid
static auto const fun_17 = c2py::dispatcher_f_kw_t{c2py::cfun_B<h5::group>(c2py::castmc<>(&h5::group::is_valid))};

// keys
static auto const fun_18 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::group::keys))};

// name
static auto const fun_19 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<>(&h5::group::name))};

// open_group
static auto const fun_20 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &>(&h5::group::open_group), "key")};

// read_attribute
static auto const fun_21 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &>(&h5::group::read_attribute), "name")};

// read_hdf5_format_from_key
static auto const fun_22 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &>(&h5::group::read_hdf5_format_from_key), "key")};

// unlink
static auto const fun_23 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &, bool>(&h5::group::unlink), "key", "error_if_absent"_a = false)};

// write_attribute
static auto const fun_24   = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::castmc<const std::string &, const std::string &>(&h5::group::write_attribute), "key", "val")};
static const auto doc_d_6  = fun_6.doc({R"DOC(   Release the HDF5 handle and reset the object to default state (id =0).)DOC"});
static const auto doc_d_7  = fun_7.doc({R"DOC(   Create a subgroup in this group
   
   Parameters
   ----------
   
   delete_if_exists: Unlink the group if it exists
   key: The name of the subgroup. If empty, return this group.)DOC"});
static const auto doc_d_8  = fun_8.doc({R"DOC(   Create a softlink in this group
   
   Parameters
   ----------
   
   delete_if_exists: Unlink the key first if it exists.
   key: The link name that will point to the target path.
   target_key: The path the link should point to. Has to exist.)DOC"});
static const auto doc_d_9  = fun_9.doc({R"DOC(   Returns all names of dataset of G)DOC"});
static const auto doc_d_10 = fun_10.doc({R"DOC(   Returns all names of dataset of G)DOC"});
static const auto doc_d_11 = fun_11.doc({R"DOC(   Returns all names of subgroup of G)DOC"});
static const auto doc_d_12 = fun_12.doc({R"DOC(   Access to the parent file)DOC"});
static const auto doc_d_13 = fun_13.doc({R"DOC(   Get the current number of references)DOC"});
static const auto doc_d_14 = fun_14.doc({R"DOC(   True iff key is a dataset of this.
   
   Parameters
   ----------
   
   key:)DOC"});
static const auto doc_d_15 = fun_15.doc({R"DOC(   True iff key is an object in the group
   
   Parameters
   ----------
   
   key:)DOC"});
static const auto doc_d_16 = fun_16.doc({R"DOC(   True iff key is a subgroup of this.
   
   Parameters
   ----------
   
   key:)DOC"});
static const auto doc_d_17 = fun_17.doc({R"DOC(   Ensure the id is valid (by H5Iis_valid).)DOC"});
static const auto doc_d_18 = fun_18.doc({R"DOC(   Returns all names of dataset of G)DOC"});
static const auto doc_d_19 = fun_19.doc({R"DOC(   Name of the group)DOC"});
static const auto doc_d_20 = fun_20.doc({R"DOC(   Open a subgroup.
   
   Throws std::runtime_error if it does not exist.
   
   Parameters
   ----------
   
   key: The name of the subgroup. If empty, return this group)DOC"});
static const auto doc_d_21 = fun_21.doc({R"DOC(   Read an attribute)DOC"});
static const auto doc_d_22 = fun_22.doc({R"DOC(   Read the format string from the key in the group)DOC"});
static const auto doc_d_23 = fun_23.doc({R"DOC(   Unlinks the subgroup key if it exists
   
   No error is thrown if key does not exists NB : unlink is almost a remove, but it does not really remove from the file (memory is not freed). After unlinking a large object, a h5repack may be needed. Cf HDF5 documentation.
   
   Parameters
   ----------
   
   error_if_absent: If True, throws an error if the key is missing.
   key: The name of the subgroup to be removed.)DOC"});
static const auto doc_d_24 = fun_24.doc({R"DOC(   Write an attribute)DOC"});

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<h5::group>[] = {
   {"close", (PyCFunction)c2py::pyfkw<fun_6>, METH_VARARGS | METH_KEYWORDS, doc_d_6.c_str()},
   {"create_group", (PyCFunction)c2py::pyfkw<fun_7>, METH_VARARGS | METH_KEYWORDS, doc_d_7.c_str()},
   {"create_softlink", (PyCFunction)c2py::pyfkw<fun_8>, METH_VARARGS | METH_KEYWORDS, doc_d_8.c_str()},
   {"get_all_dataset_names", (PyCFunction)c2py::pyfkw<fun_9>, METH_VARARGS | METH_KEYWORDS, doc_d_9.c_str()},
   {"get_all_subgroup_dataset_names", (PyCFunction)c2py::pyfkw<fun_10>, METH_VARARGS | METH_KEYWORDS, doc_d_10.c_str()},
   {"get_all_subgroup_names", (PyCFunction)c2py::pyfkw<fun_11>, METH_VARARGS | METH_KEYWORDS, doc_d_11.c_str()},
   {"get_file", (PyCFunction)c2py::pyfkw<fun_12>, METH_VARARGS | METH_KEYWORDS, doc_d_12.c_str()},
   {"get_ref_count", (PyCFunction)c2py::pyfkw<fun_13>, METH_VARARGS | METH_KEYWORDS, doc_d_13.c_str()},
   {"has_dataset", (PyCFunction)c2py::pyfkw<fun_14>, METH_VARARGS | METH_KEYWORDS, doc_d_14.c_str()},
   {"has_key", (PyCFunction)c2py::pyfkw<fun_15>, METH_VARARGS | METH_KEYWORDS, doc_d_15.c_str()},
   {"has_subgroup", (PyCFunction)c2py::pyfkw<fun_16>, METH_VARARGS | METH_KEYWORDS, doc_d_16.c_str()},
   {"is_valid", (PyCFunction)c2py::pyfkw<fun_17>, METH_VARARGS | METH_KEYWORDS, doc_d_17.c_str()},
   {"keys", (PyCFunction)c2py::pyfkw<fun_18>, METH_VARARGS | METH_KEYWORDS, doc_d_18.c_str()},
   {"name", (PyCFunction)c2py::pyfkw<fun_19>, METH_VARARGS | METH_KEYWORDS, doc_d_19.c_str()},
   {"open_group", (PyCFunction)c2py::pyfkw<fun_20>, METH_VARARGS | METH_KEYWORDS, doc_d_20.c_str()},
   {"read_attribute", (PyCFunction)c2py::pyfkw<fun_21>, METH_VARARGS | METH_KEYWORDS, doc_d_21.c_str()},
   {"read_hdf5_format_from_key", (PyCFunction)c2py::pyfkw<fun_22>, METH_VARARGS | METH_KEYWORDS, doc_d_22.c_str()},
   {"unlink", (PyCFunction)c2py::pyfkw<fun_23>, METH_VARARGS | METH_KEYWORDS, doc_d_23.c_str()},
   {"write_attribute", (PyCFunction)c2py::pyfkw<fun_24>, METH_VARARGS | METH_KEYWORDS, doc_d_24.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<h5::group>[] = {

   {nullptr, nullptr, nullptr, nullptr, nullptr}};

// ==================== module functions ====================

// h5_read
static auto const fun_25 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::cast<h5::group, const std::string &>(&h5::h5_read_bare), "g", "name")};

// h5_write
static auto const fun_26 = c2py::dispatcher_f_kw_t{c2py::cfun(c2py::cast<h5::group, const std::string &, PyObject *>(&h5::h5_write_bare), "g", "name", "ob")};

static const auto doc_d_25 = fun_25.doc({R"DOC(   )DOC"});
static const auto doc_d_26 = fun_26.doc({R"DOC(   )DOC"});
//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {"h5_read", (PyCFunction)c2py::pyfkw<fun_25>, METH_VARARGS | METH_KEYWORDS, doc_d_25.c_str()},
   {"h5_write", (PyCFunction)c2py::pyfkw<fun_26>, METH_VARARGS | METH_KEYWORDS, doc_d_26.c_str()},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "_h5py",           /* name of module */
                                        R"RAWDOC()RAWDOC", /* module documentation, may be NULL */
                                        -1,                /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit__h5py() {

  if (not c2py::check_python_version()) return NULL;

    // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<h5::file>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<h5::group>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  CLAIR_C2PY_ADD_TYPE_OBJECT(h5::file, "File");
  CLAIR_C2PY_ADD_TYPE_OBJECT(h5::group, "Group");

  return m;
}
