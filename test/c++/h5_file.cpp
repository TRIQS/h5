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
// Authors: Thomas Hahn

#include <gtest/gtest.h>
#include <h5/h5.hpp>

#include <filesystem>
#include <string>

TEST(H5, FileOperations) {
  // test various ways to construct a file
  std::string fname{"file.h5"};

  // create file in write mode
  {
    h5::file file(fname, 'w');
    EXPECT_TRUE(file.is_valid());
    EXPECT_EQ(file.name(), fname);
  }

  // overwrite file in write mode
  {
    h5::file file(fname, 'w');
    EXPECT_TRUE(file.is_valid());
  }

  // open file in read mode
  {
    h5::file file(fname, 'r');
    EXPECT_TRUE(file.is_valid());
  }

  // open file in append mode
  {
    h5::file file(fname, 'a');
    EXPECT_TRUE(file.is_valid());
  }

  // create file in append mode
  {
    EXPECT_TRUE(std::filesystem::remove(fname));
    h5::file file(fname, 'a');
    EXPECT_TRUE(file.is_valid());
    EXPECT_EQ(file.name(), fname);
  }

  // create a new file with H5F_ACC_EXCL flag
  {
    EXPECT_TRUE(std::filesystem::remove(fname));
    h5::file file(fname, 'e');
    EXPECT_TRUE(file.is_valid());
    EXPECT_EQ(file.name(), fname);
  }

  // try to create file with H5F_ACC_EXCL flag which already exists
  { EXPECT_THROW(h5::file file(fname, 'e'), std::runtime_error); }
};
