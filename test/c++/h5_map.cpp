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

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <map>

TEST(H5, Map) {

  // write
  std::map<std::string, int> m                  = {{"a", 1}, {"b", 2}};
  std::map<std::string, std::vector<double>> mv = {{"a", {1.0, 2.0}}, {"b", {2.0, 3.0, 4.0}}};

  {
    h5::file file{"test_map.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "map_int", m);
    h5_write(grp, "map_vec", mv);
  }

  // read
  std::map<std::string, int> mm;
  std::map<std::string, std::vector<double>> mmv;

  {
    h5::file file{"test_map.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "map_int", mm);
    h5_read(grp, "map_vec", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}
