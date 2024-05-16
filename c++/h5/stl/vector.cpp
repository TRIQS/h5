// Copyright (c) 2019-2024 Simons Foundation
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
// Authors: Philipp Dumitrescu, Thomas Hahn, Olivier Parcollet, Nils Wentzell

/**
 * @file
 * @brief Implementation details for stl/vector.hpp.
 */

#include "./vector.hpp"

#include <hdf5.h>
#include <hdf5_hl.h>

#include <algorithm>
#include <cstring>
#include <algorithm>

namespace h5 {

  char_buf to_char_buf(std::vector<std::string> const &v) {
    // get size of longest string
    size_t s = 1;
    for (auto &x : v) s = std::max(s, x.size() + 1);

    // copy each string to a buffer and pad with zeros
    std::vector<char> buf;
    buf.resize(std::max(v.size() * s, 1ul), 0x00);
    size_t i = 0;
    for (auto &x : v) {
      strcpy(&buf[i * s], x.c_str());
      ++i;
    }

    // return char_buf
    auto len = v_t{v.size(), s};
    return {buf, len};
  }

  char_buf to_char_buf(std::vector<std::vector<std::string>> const &v) {
    // get size of longest vector and string
    size_t s = 1, lv = 0;
    for (auto &v1 : v) {
      lv = std::max(lv, v1.size());
      for (auto &x : v1) s = std::max(s, x.size() + 1);
    }

    // copy each string to a buffer and pad with zeros
    std::vector<char> buf;
    buf.resize(std::max(v.size() * lv * s, 1ul), 0x00);
    for (int i = 0, k = 0; i < v.size(); i++)
      for (int j = 0; j < lv; j++, k++) {
        if (j < v[i].size()) strcpy(&buf[k * s], v[i][j].c_str());
      }

    // return char_buf
    auto len = v_t{v.size(), lv, s};
    return {buf, len};
  }

  void from_char_buf(char_buf const &cb, std::vector<std::string> &v) {
    // prepare vector
    v.clear();
    v.resize(cb.lengths[0]);

    // loop over all strings
    auto len_string = cb.lengths[1];
    long i          = 0;
    for (auto &x : v) {
      // use full range from char_buf and remove null characters
      const char *bptr = &cb.buffer[i * len_string];
      x                = std::string(bptr, bptr + len_string);
      x.erase(std::remove(begin(x), end(x), '\0'), end(x));
      ++i;
    }
  }

  void from_char_buf(char_buf const &cb, std::vector<std::vector<std::string>> &v) {
    // prepare vector
    v.clear();
    v.resize(cb.lengths[0]);

    // loop over all vectors and all strings
    auto inner_vec_size = cb.lengths[1];
    auto len_string     = cb.lengths[2];
    long i              = 0;
    for (auto &v_inner : v) {
      for (int j = 0; j < inner_vec_size; ++j, ++i) {
        // use full range from char_buf and remove null characters
        const char *bptr = &cb.buffer[i * len_string];
        auto s           = std::string(bptr, bptr + len_string);
        s.erase(std::remove(begin(s), end(s), '\0'), end(s));
        v_inner.push_back(s);
      }
    }
  }

  void h5_write_attribute(object obj, std::string const &name, std::vector<std::string> const &v) { h5_write_attribute(obj, name, to_char_buf(v)); }

  void h5_write_attribute(object obj, std::string const &name, std::vector<std::vector<std::string>> const &v) {
    h5_write_attribute(obj, name, to_char_buf(v));
  }

  void h5_read_attribute(object obj, std::string const &name, std::vector<std::string> &v) {
    char_buf cb;
    h5_read_attribute(obj, name, cb);
    from_char_buf(cb, v);
  }

  void h5_read_attribute(object obj, std::string const &name, std::vector<std::vector<std::string>> &v) {
    char_buf cb;
    h5_read_attribute(obj, name, cb);
    from_char_buf(cb, v);
  }

} // namespace h5
