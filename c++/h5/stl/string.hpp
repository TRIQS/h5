#pragma once
#include "../group.hpp"
#include "../format.hpp"
#include <string>

namespace h5 {

  H5_SPECIALIZE_FORMAT2(std::string, string);

  /**
   * Write a string into an h5::group
   *
   * Format : Fixed size string
   *
   * @param g The h5::group
   * @param name The name of the dataset
   * @param s String to be saved.
   */
  void h5_write(group g, std::string const &name, std::string const &s);

  /// Write a const char array as a string
  inline void h5_write(group g, std::string const &name, const char *s) { h5_write(g, name, std::string{s}); }

  /**
   * Read a string from an h5::group
   *
   * @param g The h5::group
   * @param name The name of the dataset
   * @param s The string to read into
   */
  void h5_read(group g, std::string const &name, std::string &s);

  // Explicitly forbidden.
  inline void h5_read(group g, std::string const &name, char *s) = delete;

  /**
   * Write a string attribute to an object
   *
   * @param id The object id
   * @param name The name of the attribute
   * @param s The string attribute
  */
  void h5_write_attribute(hid_t id, std::string const &name, std::string const &s);

  /// Write a const char array as a string attribute of an h5::h5_object
  inline void h5_write_attribute(hid_t id, std::string const &name, const char *s) { h5_write_attribute(id, name, std::string{s}); }

  /**
   * Read a string attribute from an object
   *
   * @param id The object id
   * @param name The name of the attribute
   * @param value The string to read into
   */
  void h5_read_attribute(hid_t id, std::string const &name, std::string &s);

  // Explicitly forbidden
  inline void h5_read_attribute(hid_t id, std::string const &name, char *s) = delete;

  /**
   * Write a string attribute to a key in an h5::group
   *
   * @param g The h5::group
   * @param key The key
   * @param name The name of the attribute
   * @param s The string attribute
  */
  void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, std::string const &s);

  /// Write a const char array as a string attribute of a particular key
  inline void h5_write_attribute_to_key(group g, std::string const &key, std::string const &name, const char *s) {
    h5_write_attribute_to_key(g, key, name, std::string{s});
  }

  /**
   * Read a string attribute from a key in an h5::group
   *
   * @param g The h5::group
   * @param key The key
   * @param name The name of the attribute
   * @param s The string to read into
  */
  void h5_read_attribute_from_key(group g, std::string const &key, std::string const &name, std::string &s);

  // ---------------------  hdf5 format -----------------------

  inline void write_hdf5_format_as_string(hid_t id, std::string const &s) { h5_write_attribute(id, "TRIQS_HDF5_data_scheme", s); }

  // Add the h5 format tag to the group
  template <typename T>
  inline void write_hdf5_format(hid_t id, T const &) {
    h5_write_attribute(id, "TRIQS_HDF5_data_scheme", get_hdf5_format<T>());
  }

  // Add the h5 format tag to the group
  inline void read_hdf5_format(hid_t id, std::string &s) { h5_read_attribute(id, "TRIQS_HDF5_data_scheme", s); }

  // Add the h5 format tag to the key in the group
  template <typename T>
  inline void write_hdf5_format_to_key(group g, std::string const &key, T const &) {
    h5_write_attribute_to_key(g, key, "TRIQS_HDF5_data_scheme", get_hdf5_format<T>());
  }

  // Add the h5 format tag to the key in the group
  inline void read_hdf5_format_from_key(group g, std::string const &key, std::string &s) {
    h5_write_attribute_to_key(g, key, "TRIQS_HDF5_data_scheme", s);
  }

  // ---------------------   char_buf -----------------------

  // char_buf contains an n dimensional array of strings as fixed size strings, flatten in a 1d array of char.
  // the last dimension is the max length of the strings + 1, because of the ending 0 in C !
  struct char_buf {
    std::vector<char> buffer;
    v_t lengths;

    // the string datatype
    [[nodiscard]] datatype dtype() const;

    // the dataspace (without last dim, which is the string).
    [[nodiscard]] dataspace dspace() const;
  };

  // read/write for char_buf
  void h5_write(group g, std::string const &name, char_buf const &cb);
  void h5_write_attribute(hid_t id, std::string const &name, char_buf const &cb);
  void h5_read(group g, std::string const &name, char_buf &_cb);
  void h5_read_attribute(hid_t id, std::string const &name, char_buf &_cb);

} // namespace h5
