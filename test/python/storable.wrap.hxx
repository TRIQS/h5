#include <c2py/c2py.hpp>

#ifndef C2PY_HXX_DECLARATION_storable_GUARDS
#define C2PY_HXX_DECLARATION_storable_GUARDS
template <>
constexpr bool c2py::is_wrapped<storable> = true;
template <>
inline constexpr auto c2py::tp_name<storable> = "storable.Storable";
#endif