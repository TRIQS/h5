
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
#include "h5_storable.cpp"
#include <c2py/serialization/h5.hpp>

using c2py::operator"" _a;

// ==================== Wrapped classes =====================

#ifndef C2PY_HXX_DECLARATION_h5_storable_GUARDS
#define C2PY_HXX_DECLARATION_h5_storable_GUARDS
template <> inline static constexpr bool c2py::is_wrapped<storable> = true;
#endif

// ==================== enums =====================

// ==================== module classes =====================

template <> inline const std::string c2py::cpp_name<storable>   = "storable";
template <> inline constexpr auto c2py::tp_name<storable>       = "h5_storable.Storable";
template <> inline constexpr const char *c2py::tp_doc<storable> = R"DOC(   )DOC";

static auto init_0                                     = c2py::dispatcher_c_kw_t{c2py::c_constructor<storable>()};
template <> constexpr initproc c2py::tp_init<storable> = c2py::pyfkw_constructor<init_0>;

// ----- Method table ----
template <>
PyMethodDef c2py::tp_methods<storable>[] = {
   {"__write_hdf5__", c2py::tpxx_write_h5<storable>, METH_VARARGS, "  "},
   {"__getstate__", c2py::getstate_h5<storable>, METH_NOARGS, ""},
   {"__setstate__", c2py::setstate_h5<storable>, METH_O, ""},
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

constexpr auto doc_member_0 = R"DOC()DOC";
constexpr auto doc_member_1 = R"DOC()DOC";

// ----- Method table ----

template <>
constinit PyGetSetDef c2py::tp_getset<storable>[] = {c2py::getsetdef_from_member<&storable::vec, storable>("vec", doc_member_0),
                                                     c2py::getsetdef_from_member<&storable::s, storable>("s", doc_member_1),

                                                     {nullptr, nullptr, nullptr, nullptr, nullptr}};

// ==================== module functions ====================

//--------------------- module function table  -----------------------------

static PyMethodDef module_methods[] = {
   {nullptr, nullptr, 0, nullptr} // Sentinel
};

//--------------------- module struct & init error definition ------------

//// module doc directly in the code or "" if not present...
/// Or mandatory ?
static struct PyModuleDef module_def = {PyModuleDef_HEAD_INIT,
                                        "h5_storable",     /* name of module */
                                        R"RAWDOC()RAWDOC", /* module documentation, may be NULL */
                                        -1,                /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                                        module_methods,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL};

//--------------------- module init function -----------------------------

extern "C" __attribute__((visibility("default"))) PyObject *PyInit_h5_storable() {

  if (not c2py::check_python_version()) return NULL;

    // import numpy iff 'numpy/arrayobject.h' included
#ifdef Py_ARRAYOBJECT_H
  import_array();
#endif

  PyObject *m;

  if (PyType_Ready(&c2py::wrap_pytype<c2py::py_range>) < 0) return NULL;
  if (PyType_Ready(&c2py::wrap_pytype<storable>) < 0) return NULL;

  m = PyModule_Create(&module_def);
  if (m == NULL) return NULL;

  auto &conv_table = *c2py::conv_table_sptr.get();

  conv_table[std::type_index(typeid(c2py::py_range)).name()] = &c2py::wrap_pytype<c2py::py_range>;
  CLAIR_C2PY_ADD_TYPE_OBJECT(storable, "Storable");

  c2py::pyref module = c2py::pyref::module("h5.formats");
  if (not module) return nullptr;
  c2py::pyref register_class = module.attr("register_class");

  register_h5_type<storable>(register_class);

  return m;
}
