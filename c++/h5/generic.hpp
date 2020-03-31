#pragma once
#include <type_traits>

namespace h5 {

  /**
   * A generic read of a key in a group
   *
   * @tparam T The C++ type to be read
   * @param g The h5::group
   * @param key Name of the element
   * @return The value read from the file
   */
  template <typename T>
  T h5_read(group g, std::string const &key) {
    if constexpr (std::is_default_constructible_v<T>) {
      T x;
      h5_read(g, key, x);
      return x;
    } else {
      return T::h5_read_construct(g, key);
    }
  }

  /**
   * A generic attribute read from an object
   *
   * @tparam T The C++ type of the attribute
   * @param id The object id
   * @param name Name of the attribute
   * @return The attribute object, and "" if the attribute does not exist.
   */
  template <typename T>
  T h5_read_attribute(hid_t id, std::string const &name) {
    T x;
    h5_read_attribute(id, name, x);
    return x;
  }

  /**
   * A generic attribute read from a key in an h5::group
   *
   * @tparam T The C++ type of the attribute
   * @param g The h5::group
   * @param key The key
   * @param name Name of the attribute
   * @return The attribute object, and "" if the attribute does not exist.
   */
  template <typename T>
  T h5_read_attribute_from_key(group g, std::string const &key, std::string const &name) {
    T x;
    h5_read_attribute_from_key(g, key, name, x);
    return x;
  }

  /**
   * Try a generic read of a key in a group.
   *
   * @tparam T The C++ type to be read
   * @param g  HDF5 group
   * @param key Name of the element
   * @param x Parameter to read.
   * @return true if the read succeeds, false if it fails
   */
  template <typename T>
  inline bool h5_try_read(group g, std::string key, T &x) {
    if (g.has_key(key)) {
      h5_read(g, key, x);
      return true;
    }
    return false;
  }

} // namespace h5
