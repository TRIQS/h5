// Copyright (c) 2019-2022 Simons Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0.txt
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: Olivier Parcollet, Nils Wentzell

#ifndef LIBH5_SCALAR_HPP
#define LIBH5_SCALAR_HPP

#include <cmath>
#include "./group.hpp"
#include "./array_interface.hpp"
namespace h5 {

  namespace array_interface {

    template <typename S>
    h5_array_view h5_array_view_from_scalar(S &&s) {
      return {hdf5_type<std::decay_t<S>>(), (void *)(&s), 0, is_complex_v<std::decay_t<S>>};
    }
  } // namespace array_interface

  template <typename T>
  void h5_write(group g, std::string const &name, T const &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T> or std::is_same_v<T, dcplx_t>) {
    array_interface::write(g, name, array_interface::h5_array_view_from_scalar(x), false);
  }

  template <typename T>
  void h5_read(group g, std::string const &name, T &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T> or std::is_same_v<T, dcplx_t>) {

    if constexpr (is_complex_v<T>) {
      // Backward compatibility to read complex stored the old way
      if (g.has_subgroup(name)) {
        group gr = g.open_group(name);
        H5_ASSERT(gr.has_key("r") and gr.has_key("i"));
        double r = NAN, i = NAN;
        h5_read(gr, "r", r);
        h5_read(gr, "i", i);
        x = std::complex<double>{r, i};
        return;
      }
    }

    auto lt = array_interface::get_h5_lengths_type(g, name);

    if constexpr (is_complex_v<T>) {
      // Allow reading compound hdf5 dataype into complex
      if (hdf5_type_equal(lt.ty, hdf5_type<dcplx_t>())) {
        h5_read(g, name, reinterpret_cast<dcplx_t &>(x));
        return;
      }
    }

    array_interface::read(g, name, array_interface::h5_array_view_from_scalar(x), lt);
  }

  template <typename T>
  void h5_write_attribute(object obj, std::string const &name, T const &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T>) {
    array_interface::write_attribute(obj, name, array_interface::h5_array_view_from_scalar(x));
  }

  template <typename T>
  void h5_read_attribute(object obj, std::string const &name, T &x) H5_REQUIRES(std::is_arithmetic_v<T> or is_complex_v<T>) {
    array_interface::read_attribute(obj, name, array_interface::h5_array_view_from_scalar(x));
  }

} // namespace h5

#endif // LIBH5_SCALAR_HPP
