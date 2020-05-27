#pragma once

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
