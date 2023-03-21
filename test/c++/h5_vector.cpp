// Copyright (c) 2020 Simons Foundation
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
// Authors: Nils Wentzell

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <variant>

TEST(H5, Vector) {

  // write
  std::vector<std::string> vstr = {"a", "b"};
  std::vector<double> vdbl      = {1.0, 2.0};
  std::vector<dcomplex> vcplx   = {{1.1, 2.2}, {3.3, 4.5}};

  {
    h5::file file{"test_vec.h5", 'w'};
    h5::group grp{file};
    h5::write(grp, "vec_str", vstr);
    h5::write(grp, "vec_dbl", vdbl);
    h5::write(grp, "vec_cplx", vcplx);
  }

  // read
  std::vector<std::string> vstr_read;
  std::vector<double> vdbl_read;
  std::vector<dcomplex> vcplx_read;

  {
    h5::file file{"test_vec.h5", 'r'};
    h5::read(file, "vec_str", vstr_read);
    h5::read(file, "vec_dbl", vdbl_read);
    h5::read(file, "vec_cplx", vcplx_read);
  }

  // compare
  EXPECT_EQ(vstr, vstr_read);
  EXPECT_EQ(vdbl, vdbl_read);
  EXPECT_EQ(vcplx, vcplx_read);
}

TEST(H5, VectorEmpty) {

  // write
  std::vector<double> v = {};

  {
    h5::file file{"test_vec_empty.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "vec", v);
  }

  // read
  std::vector<double> vv;

  {
    h5::file file{"test_vec_empty.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "vec", vv);
  }

  // compare
  EXPECT_EQ(v, vv);
}

TEST(H5, VectorAttribute) {
  // write
  std::vector<std::vector<std::string>> vvs  = {{"a", "b"}, {"c", "d"}, {"e", "f"}};
  std::vector<std::vector<std::string>> evvs = {};
  std::vector<std::vector<std::string>> vevs = {{}, {}, {}};
  std::vector<std::vector<std::string>> vves = {{"", ""}, {"", ""}, {"", ""}};

  {
    h5::file file{"test_attribute.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "vec", 0);

    auto ds = grp.open_dataset("vec");
    h5_write_attribute(ds, "attr_vvs", vvs);
    h5_write_attribute(ds, "attr_evvs", evvs);
    h5_write_attribute(ds, "attr_vevs", vevs);
    h5_write_attribute(ds, "attr_vves", vves);
  }

  // read
  std::vector<std::vector<std::string>> rvvs, revvs, rvevs, rvves;

  {
    h5::file file{"test_attribute.h5", 'r'};
    h5::group grp{file};

    auto ds = grp.open_dataset("vec");
    h5_read_attribute(ds, "attr_vvs", rvvs);
    h5_read_attribute(ds, "attr_evvs", revvs);
    h5_read_attribute(ds, "attr_vevs", rvevs);
    h5_read_attribute(ds, "attr_vves", rvves);
  }

  // compare
  EXPECT_EQ(vvs, rvvs);
  EXPECT_EQ(evvs, revvs);
  EXPECT_EQ(vevs, rvevs);
  EXPECT_EQ(vves, rvves);
}
