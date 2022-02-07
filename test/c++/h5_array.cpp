// Copyright (c) 2020-2022 Simons Foundation
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
#include <array>
#include <string>
#include <complex>

TEST(H5, Array) {

  // write
  auto arr_str = std::array<std::string, 2>{"a", "abc"};
  auto arr_dbl = std::array<double, 2>{1.0, 2.0};

  {
    h5::file file{"test_arr.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "arr_str", arr_str);
    h5_write(grp, "arr_dbl", arr_dbl);
  }

  // read
  std::array<std::string, 2> arr_str_r;
  std::array<double, 2> arr_dbl_r{};

  {
    h5::file file{"test_arr.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "arr_str", arr_str_r);
    h5_read(grp, "arr_dbl", arr_dbl_r);
  }

  // compare
  EXPECT_EQ(arr_str, arr_str_r);
  EXPECT_EQ(arr_dbl, arr_dbl_r);
}

TEST(H5, ArrayConvert) {

  // write
  auto arr_int = std::array<int, 2>{1, 2};
  auto arr_dbl = std::array<double, 2>{1.5, 2.5};

  {
    h5::file file{"test_arr_convert.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "arr_int", arr_int);
    h5_write(grp, "arr_dbl", arr_dbl);
  }

  // read
  std::array<long, 2> arr_long{};
  std::array<dcomplex, 2> arr_cplx;

  {
    h5::file file{"test_arr_convert.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "arr_int", arr_long); // Issues Warning
    h5_read(grp, "arr_dbl", arr_cplx); // Issues Warning
  }

  // compare
  EXPECT_EQ((std::array{1l, 2l}), arr_long);
  using namespace std::literals;
  EXPECT_EQ((std::array{1.5 + 0i, 2.5 + 0i}), arr_cplx);
}
