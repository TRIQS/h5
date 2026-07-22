// Copyright (c) 2020-2024 Simons Foundation
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
// Authors: Thomas Hahn, Nils Wentzell, chuffa

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <complex>
#include <map>
#include <string>
#include <utility>
#include <vector>

TEST(H5, MapWithStringKeyType) {
  // write/read a map with string keys and ints and vectors of doubles as values
  std::map<std::string, int> m_int                 = {{"a", 1}, {"b", 2}};
  std::map<std::string, std::vector<double>> m_vec = {{"a", {1.0, 2.0}}, {"b", {2.0, 3.0, 4.0}}};

  {
    h5::file file{"test_map.h5", 'w'};
    h5::write(file, "map_int", m_int);
    h5::write(file, "map_vec", m_vec);
  }

  {
    h5::file file{"test_map.h5", 'r'};

    std::map<std::string, int> m_int_in;
    std::map<std::string, std::vector<double>> m_vec_in;
    h5::read(file, "map_int", m_int_in);
    h5::read(file, "map_vec", m_vec_in);

    EXPECT_EQ(m_int, m_int_in);
    EXPECT_EQ(m_vec, m_vec_in);
  }
}

TEST(H5, MapWithNonStrKeyType) {
  // write/read maps with non-string keys (stored with the "DictNonStrKey" tag): pairs of
  // strings and plain scalars. This also writes the reference archive read by the Python
  // test_dict_*_from_cpp tests (regenerated via test/c++/gen_dict_nonstrkey_refs.sh).
  std::map<std::pair<std::string, std::string>, int> m_int                 = {{{"a", "b"}, 1}, {{"c", "d"}, 2}};
  std::map<std::pair<std::string, std::string>, std::vector<double>> m_vec = {{{"a", "b"}, {1.0, 2.0}}, {{"c", "d"}, {3.0, 4.0}}};
  std::map<int, int> m_scalar                                              = {{1, 10}, {2, 20}};

  {
    h5::file file{"test_map_pairkey.h5", 'w'};
    h5::write(file, "map_int", m_int);
    h5::write(file, "map_vec", m_vec);
    h5::write(file, "map_scalar", m_scalar);
  }

  {
    h5::file file{"test_map_pairkey.h5", 'r'};

    // a non-string key type uses the "DictNonStrKey" tag (vs. "Dict" for string keys)
    h5::group root(file);
    EXPECT_EQ(root.read_hdf5_format_from_key("map_int"), "DictNonStrKey");
    EXPECT_EQ(root.read_hdf5_format_from_key("map_scalar"), "DictNonStrKey");

    std::map<std::pair<std::string, std::string>, int> m_int_in;
    std::map<std::pair<std::string, std::string>, std::vector<double>> m_vec_in;
    std::map<int, int> m_scalar_in;
    h5::read(file, "map_int", m_int_in);
    h5::read(file, "map_vec", m_vec_in);
    h5::read(file, "map_scalar", m_scalar_in);

    EXPECT_EQ(m_int, m_int_in);
    EXPECT_EQ(m_vec, m_vec_in);
    EXPECT_EQ(m_scalar, m_scalar_in);
  }
}

TEST(H5, MapWithNonStrKeyFromPython) {
  // cross-language read: a DictNonStrKey archive written by the Python h5 module
  // (test/c++/gen_dict_tuplekey.py) must read back into the equivalent std::map, for
  // both tuple keys (std::pair) and scalar keys.
  std::map<std::pair<std::string, std::string>, long> const m_int_ref                = {{{"a", "b"}, 1}, {{"c", "d"}, 2}};
  std::map<std::pair<std::string, std::string>, std::vector<double>> const m_vec_ref = {{{"a", "b"}, {1.0, 2.0}}, {{"c", "d"}, {3.0, 4.0}}};
  std::map<long, long> const m_scalar_ref                                            = {{1, 10}, {2, 20}};

  h5::file file{"dict_tuplekey.ref.h5", 'r'};

  std::map<std::pair<std::string, std::string>, long> m_int;
  std::map<std::pair<std::string, std::string>, std::vector<double>> m_vec;
  std::map<long, long> m_scalar;
  h5::read(file, "map_int", m_int);
  h5::read(file, "map_vec", m_vec);
  h5::read(file, "map_scalar", m_scalar);

  EXPECT_EQ(m_int, m_int_ref);
  EXPECT_EQ(m_vec, m_vec_ref);
  EXPECT_EQ(m_scalar, m_scalar_ref);
}

TEST(H5, MapWithGreaterComparison) {
  // write/read a map with int keys and std::complex<double> values using std::greater as comparison
  std::map<int, std::complex<double>, std::greater<int>> m_complex = {{1, {1.0, 2.0}}, {2, {3.0, 4.0}}};

  {
    h5::file file{"test_map_greater.h5", 'w'};
    h5::write(file, "map_complex", m_complex);
  }

  {
    h5::file file{"test_map_greater.h5", 'r'};

    std::map<int, std::complex<double>, std::greater<int>> m_complex_in;
    h5::read(file, "map_complex", m_complex_in);

    EXPECT_EQ(m_complex, m_complex_in);
  }
}

// Custom type to be used as a key in a map.
class custom_key_class {
  int var;

  public:
  custom_key_class(int v = 0) : var(v) {};

  static std::string hdf5_format() { return "TestkeyClass"; }

  friend void h5_write(h5::group g, std::string const &name, custom_key_class const &c) { h5_write(g, name, c.var); };

  friend void h5_read(h5::group g, std::string const &name, custom_key_class &c) { h5_read(g, name, c.var); };

  bool operator<(const custom_key_class &other) const { return var < other.var; }

  bool operator==(const custom_key_class &other) const { return var == other.var; }
};

TEST(H5, MapWithCustomKeyType) {
  // write/read a map with custom key type and string values
  std::map<custom_key_class, std::string> m;
  m.emplace(custom_key_class(1), "hey");
  m.emplace(custom_key_class(10), "you");

  {
    h5::file file{"test_map_2.h5", 'w'};
    h5::write(file, "map_customClass", m);
  }

  {
    h5::file file{"test_map_2.h5", 'r'};

    std::map<custom_key_class, std::string> m_in;
    h5::read(file, "map_customClass", m_in);

    EXPECT_EQ(m, m_in);
  }
}

// Function to write a map the old way.
template <typename T>
void h5_write_old(h5::group f, std::string const &name, std::map<std::string, T> const &m) {
  auto gr = f.create_group(name);
  h5::write_hdf5_format(gr, m); // NOLINT (slicing is intended)
  for (auto &pvp : m) h5::write(gr, pvp.first, pvp.second);
}

TEST(H5, MapOldVsNew) {
  // test backwards compatibility by writing with the old function and reading using the new function
  std::map<std::string, int> m;
  m.emplace("hey", 1);
  m.emplace("you", 15);

  {
    h5::file file{"test_map_3.h5", 'w'};
    h5_write_old(file, "map_int", m);
  }

  {
    h5::file file{"test_map_3.h5", 'r'};

    std::map<std::string, int> m_in;
    h5::read(file, "map_int", m_in);

    EXPECT_EQ(m, m_in);
  }
}
