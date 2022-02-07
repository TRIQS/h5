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

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <variant>

TEST(H5, VariantIntComplex) {

  using v_t = std::variant<int, std::complex<double>>;
  auto c    = std::complex<double>{1, 2};
  {
    auto v1 = v_t{6};
    auto v2 = v_t{c};

    h5::file file("test_variantIC.h5", 'w');
    h5_write(file, "v1", v1);
    h5_write(file, "v2", v2);
  }

  {
    v_t v1, v2;
    h5::file file("test_variantIC.h5", 'r');
    h5_read(file, "v1", v1);
    h5_read(file, "v2", v2);

    EXPECT_EQ(std::get<int>(v1), 6); // std library version
    EXPECT_EQ(std::get<std::complex<double>>(v2), c);
  }
}

// -----------------------------

TEST(H5, VariantIntString) {

  using v_t = std::variant<int, std::string>;
  auto s    = std::string{"Hello"};
  {
    auto v1 = v_t{6};
    auto v2 = v_t{s};

    h5::file file("test_variantIS.h5", 'w');
    h5_write(file, "v1", v1);
    h5_write(file, "v2", v2);
  }

  {
    v_t v1, v2;
    h5::file file("test_variantIS.h5", 'r');
    h5_read(file, "v1", v1);
    h5_read(file, "v2", v2);

    EXPECT_EQ(std::get<int>(v1), 6); // std library version
    EXPECT_EQ(std::get<std::string>(v2), s);
  }
}
