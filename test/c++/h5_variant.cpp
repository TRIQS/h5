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
// Authors: Olivier Parcollet, Nils Wentzell

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <complex>
#include <string>
#include <variant>

TEST(H5, VariantIntComplex) {
  // write/read a variant of int and complex
  using v_t = std::variant<int, std::complex<double>>;
  std::complex<double> z{1, 2};
  int i{6};

  {
    h5::file file("test_variantIC.h5", 'w');

    auto v1 = v_t{i};
    auto v2 = v_t{z};
    h5_write(file, "v1", v1);
    h5_write(file, "v2", v2);
  }

  {
    h5::file file("test_variantIC.h5", 'r');

    v_t v1, v2;
    h5_read(file, "v1", v1);
    h5_read(file, "v2", v2);

    EXPECT_EQ(std::get<int>(v1), i);
    EXPECT_EQ(std::get<std::complex<double>>(v2), z);
  }
}

TEST(H5, VariantIntString) {
  // write/read a variant of int and string
  using v_t = std::variant<int, std::string>;
  std::string s{"Hello"};
  int i{6};
  {
    h5::file file("test_variantIS.h5", 'w');

    auto v1 = v_t{i};
    auto v2 = v_t{s};
    h5_write(file, "v1", v1);
    h5_write(file, "v2", v2);
  }

  {
    h5::file file("test_variantIS.h5", 'r');

    v_t v1, v2;
    h5_read(file, "v1", v1);
    h5_read(file, "v2", v2);

    EXPECT_EQ(std::get<int>(v1), i);
    EXPECT_EQ(std::get<std::string>(v2), s);
  }
}
