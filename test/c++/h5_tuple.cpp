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

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <string>
#include <tuple>
#include <vector>

TEST(H5, Tuple) {
  // write/read a tuple of string and int and a tuple of string and vector
  std::tuple<std::string, int> tsi                 = {"a", 1};
  std::tuple<std::string, std::vector<double>> tsv = {"a", {1.0, 2.0}};

  {
    h5::file file{"test_tuple.h5", 'w'};

    h5::write(file, "tpl_int_str", tsi);
    h5::write(file, "tpl_str_vec", tsv);
  }

  {
    h5::file file{"test_tuple.h5", 'r'};

    std::tuple<std::string, int> tsi_in;
    std::tuple<std::string, std::vector<double>> tsv_in;
    h5::read(file, "tpl_int_str", tsi_in);
    h5::read(file, "tpl_str_vec", tsv_in);

    EXPECT_EQ(tsi, tsi_in);
    EXPECT_EQ(tsv, tsv_in);
  }
}
