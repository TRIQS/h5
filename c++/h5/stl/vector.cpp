#include "./vector.hpp"
#include <hdf5.h>
#include <hdf5_hl.h>
#include <array>
#include <cstring>

#include "./string.hpp"

namespace h5 {

  //------------------  to_char_buf ------------------------------

  // copy to the buffer, with each string having the same length
  char_buf to_char_buf(std::vector<std::string> const &v) {

    size_t s = 1;
    for (auto &x : v) s = std::max(s, x.size() + 1);
    auto len = v_t{v.size(), s};

    // copy to the buffer
    std::vector<char> buf;
    buf.resize(std::max(v.size() * s, 1ul), 0x00);
    size_t i = 0;
    for (auto &x : v) {
      strcpy(&buf[i * s], x.c_str());
      ++i;
    }

    return {buf, len};
  }

  // copy to the buffer, with each string having the same length
  char_buf to_char_buf(std::vector<std::vector<std::string>> const &v) {

    size_t s = 1, lv = 0;
    for (auto &v1 : v) {
      lv = std::max(lv, v1.size());
      for (auto &x : v1) s = std::max(s, x.size() + 1);
    }
    auto len = v_t{v.size(), lv, s};

    // copy to the buffer
    std::vector<char> buf;
    buf.resize(std::max(v.size() * lv * s, 1ul), 0x00);
    for (int i = 0, k = 0; i < v.size(); i++)
      for (int j = 0; j < lv; j++, k++) {
        if (j < v[i].size()) strcpy(&buf[k * s], v[i][j].c_str());
      }

    return {buf, len};
  }

  //------------------- from_char_buf-----------------------------

  void from_char_buf(char_buf const &cb, std::vector<std::string> &v) {
    v.clear();
    v.resize(cb.lengths[0]);
    auto len_string = cb.lengths[1];

    size_t i = 0;
    for (auto &x : v) {
      x = "";

      // We append the chars one to stay backward compatible
      // Certain hdf5 files contain strings without null-termination
      for (int j = 0; j < len_string; ++j) {
        char c = cb.buffer[i * len_string + j];
        if (c == 0x00) break;
        x.append(1, c);
      }
      ++i;
    }
  }

  //--------

  void from_char_buf(char_buf const &cb, std::vector<std::vector<std::string>> &v) {
    v.clear();
    v.resize(cb.lengths[0]);
    auto inner_vec_size = cb.lengths[1];
    auto len_string     = cb.lengths[2];
    long k              = 0;
    for (auto &v_inner : v) {
      for (int j = 0; j < inner_vec_size; ++j, ++k) {
        std::string s = "";

        // We append the chars one to stay backward compatible
        // Certain hdf5 files contain strings without null-termination
        for (int l = 0; l < len_string; ++l) {
          char c = cb.buffer[k * len_string + l];
          if (c == 0x00) break;
          x.append(1, c);
        }
        s.append(&cb.buffer[k * len_string]);
        v_inner.push_back(s);
      }
    }
  }

  // -----------   WRITE  ATTRIBUTE ------------

  void h5_write_attribute(hid_t id, std::string const &name, std::vector<std::string> const &v) { h5_write_attribute(id, name, to_char_buf(v)); }

  void h5_write_attribute(hid_t id, std::string const &name, std::vector<std::vector<std::string>> const &v) {
    h5_write_attribute(id, name, to_char_buf(v));
  }

  // -----------   READ  ATTRIBUTE ------------

  void h5_read_attribute(hid_t id, std::string const &name, std::vector<std::string> &v) {
    char_buf cb;
    h5_read_attribute(id, name, cb);
    from_char_buf(cb, v);
  }

  void h5_read_attribute(hid_t id, std::string const &name, std::vector<std::vector<std::string>> &v) {
    char_buf cb;
    h5_read_attribute(id, name, cb);
    from_char_buf(cb, v);
  }

} // namespace h5
