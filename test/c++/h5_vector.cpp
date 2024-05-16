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
// Authors: Thomas Hahn, Nils Wentzell

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <complex>
#include <string>
#include <vector>

TEST(H5, VectorOfBasicTypes) {
  // write/read a vector of strings, doubles, and complex doubles
  std::vector<std::string> vstr           = {"a", "b"};
  std::vector<double> vdbl                = {1.0, 2.0};
  std::vector<std::complex<double>> vcplx = {{1.1, 2.2}, {3.3, 4.5}};

  {
    h5::file file{"test_vec.h5", 'w'};
    h5::write(file, "vec_str", vstr);
    h5::write(file, "vec_dbl", vdbl);
    h5::write(file, "vec_cplx", vcplx);
  }

  {
    h5::file file{"test_vec.h5", 'r'};

    std::vector<std::string> vstr_in;
    std::vector<double> vdbl_in;
    std::vector<std::complex<double>> vcplx_in;
    h5::read(file, "vec_str", vstr_in);
    h5::read(file, "vec_dbl", vdbl_in);
    h5::read(file, "vec_cplx", vcplx_in);

    EXPECT_EQ(vstr, vstr_in);
    EXPECT_EQ(vdbl, vdbl_in);
    EXPECT_EQ(vcplx, vcplx_in);
  }
}

TEST(H5, VectorEmpty) {
  // write/read an empty vector
  std::vector<double> v = {};

  {
    h5::file file{"test_vec_empty.h5", 'w'};
    h5_write(file, "vec", v);
  }

  {
    h5::file file{"test_vec_empty.h5", 'r'};

    std::vector<double> v_in;
    h5_read(file, "vec", v_in);

    EXPECT_EQ(v, v_in);
  }
}

TEST(H5, VectorStringAttributes) {
  // write/read a vector of vectors of strings
  std::vector<std::vector<std::string>> vvs  = {{"a", "b"}, {"c", "d"}, {"e", "f"}};
  std::vector<std::vector<std::string>> evvs = {};
  std::vector<std::vector<std::string>> vevs = {{}, {}, {}};
  std::vector<std::vector<std::string>> vves = {{"", ""}, {"", ""}, {"", ""}};

  {
    h5::file file{"test_attribute.h5", 'w'};
    h5::group grp{file};
    h5::write(grp, "vec", 0);
    auto ds = grp.open_dataset("vec");

    h5::write_attribute(ds, "attr_vvs", vvs);
    h5::write_attribute(ds, "attr_evvs", evvs);
    h5::write_attribute(ds, "attr_vevs", vevs);
    h5::write_attribute(ds, "attr_vves", vves);
  }

  {
    h5::file file{"test_attribute.h5", 'r'};
    h5::group grp{file};
    auto ds = grp.open_dataset("vec");

    std::vector<std::vector<std::string>> vvs_in, evvs_in, vevs_in, vves_in;
    h5::read_attribute(ds, "attr_vvs", vvs_in);
    h5::read_attribute(ds, "attr_evvs", evvs_in);
    h5::read_attribute(ds, "attr_vevs", vevs_in);
    h5::read_attribute(ds, "attr_vves", vves_in);

    EXPECT_EQ(vvs, vvs_in);
    EXPECT_EQ(evvs, evvs_in);
    EXPECT_EQ(vevs, vevs_in);
    EXPECT_EQ(vves, vves_in);
  }
}

// Custom type.
class foo {
  int var;

  public:
  foo(int v = 0) : var(v){};

  static std::string hdf5_format() { return "foo"; }

  friend void h5_write(h5::group g, std::string const &name, foo const &f) { h5_write(g, name, f.var); };

  friend void h5_read(h5::group g, std::string const &name, foo &f) { h5_read(g, name, f.var); };

  bool operator==(const foo &other) const { return var == other.var; }
};

TEST(H5, VectorOfCustomType) {
  // write/read a vector of a custom type
  std::vector<foo> v = {1, 2, 3};

  {
    h5::file file{"test_vec_custom.h5", 'w'};
    h5::write(file, "vec_foo", v);
  }

  {
    h5::file file{"test_vec_custom.h5", 'r'};

    std::vector<foo> v_in;
    h5::read(file, "vec_foo", v_in);

    EXPECT_EQ(v, v_in);
  }
}
