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

#include <optional>

TEST(H5, Optional) {
  // write/read an optional int
  std::optional<int> a(18);

  {
    h5::file file("test_opt.h5", 'w');
    h5::write(file, "a", a);
  }

  {
    h5::file file("test_opt.h5", 'r');

    std::optional<int> b;
    h5::read(file, "a", b);

    EXPECT_TRUE(bool(b));
    EXPECT_EQ(a, b);
  }
}
