// Copyright (c) 2024 Simons Foundation
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

#include <hdf5_hl.h>

#include <string>

TEST(H5, GroupOperations) {
  // test the various group operations
  std::string fname{"group.h5"};
  std::string gname{"group"};
  std::string dsname{"dataset"};
  std::string lname{"link"};
  h5::file file(fname, 'w');

  // create root group
  h5::group root(file);
  EXPECT_TRUE(root.is_valid());
  EXPECT_EQ(root.name(), "/");

  // create subgroup
  EXPECT_FALSE(root.has_subgroup(gname));
  EXPECT_FALSE(root.has_key(gname));
  auto group = root.create_group(gname);
  EXPECT_TRUE(group.is_valid());
  EXPECT_EQ(group.name(), "/" + gname);
  EXPECT_TRUE(root.has_subgroup(gname));
  EXPECT_TRUE(root.has_key(gname));

  // open subgroup
  EXPECT_EQ(root.open_group(gname).name(), "/" + gname);
  EXPECT_THROW(std::ignore = root.open_group("nonexistent"), std::runtime_error);

  // create dataset
  auto dataspace = H5Screate(H5S_SCALAR);
  EXPECT_FALSE(root.has_dataset(dsname));
  EXPECT_FALSE(root.has_key(dsname));
  auto dataset = root.create_dataset(dsname, h5::hdf5_type<int>(), dataspace);
  EXPECT_TRUE(dataset.is_valid());
  EXPECT_TRUE(root.has_dataset(dsname));
  EXPECT_TRUE(root.has_key(dsname));

  // open dataset
  EXPECT_TRUE(root.open_dataset(dsname).is_valid());
  EXPECT_THROW(std::ignore = root.open_dataset("nonexistent"), std::runtime_error);

  // create softlink
  EXPECT_FALSE(root.has_key(lname));
  root.create_softlink(dsname, lname);
  EXPECT_TRUE(root.has_key(lname));
  root.unlink(lname);
  EXPECT_FALSE(root.has_key(lname));

  // get all dataset and subgroup names
  auto names = root.get_all_subgroup_dataset_names();
  EXPECT_EQ(names.size(), 2);
  for (const auto &n : names) { EXPECT_TRUE(n == gname || n == dsname); }
};
