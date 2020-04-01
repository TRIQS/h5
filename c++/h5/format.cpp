#include "./format.hpp"
#include "./stl/string.hpp"
#include <string>

namespace h5 {

  void read_hdf5_format(hid_t id, std::string &s) {
    h5_read_attribute(id, "Format", s);
    if (s == "") { // Backward compatibility
      h5_read_attribute(id, "TRIQS_HDF5_data_scheme", s);
    }
  }

  std::string read_hdf5_format(group g) {
    std::string s;
    read_hdf5_format(g, s);
    return s;
  }

  void read_hdf5_format_from_key(group g, std::string const &key, std::string &s) {
    h5_read_attribute_from_key(g, key, "Format", s);
    if (s == "") { // Backward compatibility
      h5_read_attribute_from_key(g, key, "TRIQS_HDF5_data_scheme", s);
    }
  }

  void assert_hdf5_format_as_string(group g, const char *tag_expected, bool ignore_if_absent) {
    auto tag_file = read_hdf5_format(g);
    if (ignore_if_absent and tag_file.empty()) return;
    if (tag_file != tag_expected)
      throw std::runtime_error("h5_read : mismatch of the Format tag in the h5 group : found " + tag_file + " while I expected " + tag_expected);
  }

} // namespace h5
