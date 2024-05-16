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

#include <string>
#include <utility>
#include <vector>

TEST(H5, Pair) {
  // write/read a pair of string and int and a pair of string and vector
  std::pair<std::string, int> psi                 = {"a", 1};
  std::pair<std::string, std::vector<double>> psv = {"a", {1.0, 2.0}};

  {
    h5::file file{"test_pair.h5", 'w'};

    h5::write(file, "pair_int_str", psi);
    h5::write(file, "pair_str_vec", psv);
  }

  {
    h5::file file{"test_pair.h5", 'r'};

    std::pair<std::string, int> psi_in;
    std::pair<std::string, std::vector<double>> psv_in;
    h5::read(file, "pair_int_str", psi_in);
    h5::read(file, "pair_str_vec", psv_in);

    EXPECT_EQ(psi, psi_in);
    EXPECT_EQ(psv, psv_in);
  }
}
