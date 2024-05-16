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

#include <array>
#include <complex>
#include <string>

TEST(H5, ArrayOfBasicTypes) {
  // write/read an array of strings and doubles
  auto arr_str = std::array<std::string, 2>{"a", "abc"};
  auto arr_dbl = std::array<double, 2>{1.0, 2.0};

  {
    h5::file file{"test_arr.h5", 'w'};

    h5::write(file, "arr_str", arr_str);
    h5::write(file, "arr_dbl", arr_dbl);
  }

  {
    h5::file file{"test_arr.h5", 'r'};

    std::array<std::string, 2> arr_str_in;
    std::array<double, 2> arr_dbl_in{};
    h5::read(file, "arr_str", arr_str_in);
    h5::read(file, "arr_dbl", arr_dbl_in);
    EXPECT_EQ(arr_str, arr_str_in);
    EXPECT_EQ(arr_dbl, arr_dbl_in);
  }
}

TEST(H5, ArrayConvert) {
  // write an array of ints and doubles and read them as longs and complex doubles
  using namespace std::complex_literals;
  auto arr_int = std::array<int, 2>{1, 2};
  auto arr_dbl = std::array<double, 2>{1.5, 2.5};

  {
    h5::file file{"test_arr_convert.h5", 'w'};

    h5::write(file, "arr_int", arr_int);
    h5::write(file, "arr_dbl", arr_dbl);
  }

  {
    h5::file file{"test_arr_convert.h5", 'r'};

    std::array<long, 2> arr_long{};
    std::array<std::complex<double>, 2> arr_cplx;
    h5::read(file, "arr_int", arr_long); // issues a warning
    h5::read(file, "arr_dbl", arr_cplx); // issues a warning

    EXPECT_EQ((std::array{1l, 2l}), arr_long);
    EXPECT_EQ((std::array{1.5 + 0i, 2.5 + 0i}), arr_cplx);
  }
}
