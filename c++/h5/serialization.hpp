// Copyright (c) 2019-2020 Simons Foundation
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

#ifndef LIBH5_SERIALIZATION_HPP
#define LIBH5_SERIALIZATION_HPP

#include "./file.hpp"
#include "./group.hpp"
#include "./generic.hpp"

namespace h5 {

  template <typename T>
  std::vector<unsigned char> serialize(T const &x) {
    memory_file f;
    h5_write(f, "object", x);
    return f.as_buffer();
  }

  // -----------------------------

  template <typename T>
  T deserialize(std::vector<unsigned char> const &buf) {
    memory_file f{buf};
    return h5_read<T>(f, "object");
  }
} // namespace h5

#endif // LIBH5_SERIALIZATION_HPP
