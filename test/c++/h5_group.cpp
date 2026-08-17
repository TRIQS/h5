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
#include <vector>

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

TEST(H5, GroupHasKeyNestedPath) {
  h5::file file("group_nested.h5", 'w');
  h5::group root(file);
  root.create_group("a").create_group("b");

  EXPECT_TRUE(root.has_key("a/b"));
  EXPECT_TRUE(root.has_key("/a/b"));
  EXPECT_TRUE(root.has_subgroup("a/b"));

  // intermediate group missing: H5Lexists fails rather than reporting absence
  EXPECT_FALSE(root.has_key("nested/group"));
  EXPECT_FALSE(root.has_key("nested/deeply/nested/group"));
  EXPECT_FALSE(root.has_key("a/nope/c"));
  EXPECT_FALSE(root.has_subgroup("nested/group"));
  EXPECT_FALSE(root.has_dataset("nested/group"));
  EXPECT_NO_THROW(root.unlink("nested/group"));

  // final component missing below an existing group
  EXPECT_FALSE(root.has_key("a/nope"));

  // dangling softlink: the link exists, the path below it does not
  root.create_softlink("/a", "lnk");
  EXPECT_TRUE(root.has_key("lnk"));
  EXPECT_TRUE(root.has_key("lnk/b"));
  root.unlink("a");
  EXPECT_TRUE(root.has_key("lnk"));
  EXPECT_FALSE(root.has_key("lnk/b"));
};

TEST(H5, GroupNameOrdering) {
  // names should be returned in increasing-name order, independent of insertion order
  h5::file file("group_order.h5", 'w');
  h5::group root(file);

  // create subgroups and datasets in a deliberately unsorted insertion order
  for (const auto &n : {"gc", "ga", "gb"}) std::ignore = root.create_group(n);
  for (const auto &n : {"dz", "dx", "dy"}) h5::write(root, n, 0);

  EXPECT_EQ(root.get_all_subgroup_names(), (std::vector<std::string>{"ga", "gb", "gc"}));
  EXPECT_EQ(root.get_all_dataset_names(), (std::vector<std::string>{"dx", "dy", "dz"}));
  EXPECT_EQ(root.get_all_subgroup_dataset_names(), (std::vector<std::string>{"dx", "dy", "dz", "ga", "gb", "gc"}));
}

TEST(H5, GroupWriteRead) {
  // test writing/reading datasets in groups
  {
    h5::file file("group_rw.h5", 'w');
    h5::group root(file);
    h5::write(root, "vec", std::vector<int>{1, 2, 3});
    h5::write_hdf5_format(root.open_dataset("vec"), std::vector<int>{});
    h5::write(root, "untagged", 0);
    root.write_attribute("attr_key", "attr_val");
  }

  {
    h5::file file("group_rw.h5", 'r');
    h5::group root(file);
    EXPECT_EQ((std::vector<int>{1, 2, 3}), h5::read<std::vector<int>>(root, "vec"));
    EXPECT_EQ("attr_val", root.read_attribute("attr_key"));
    EXPECT_EQ(h5::get_hdf5_format(std::vector<int>{}), root.read_hdf5_format_from_key("vec"));

    // an untagged dataset has no format, while an unresolvable key is an error
    EXPECT_EQ("", root.read_hdf5_format_from_key("untagged"));
    EXPECT_THROW(std::ignore = root.read_hdf5_format_from_key("nonexistent"), std::runtime_error);
    EXPECT_THROW(std::ignore = root.read_hdf5_format_from_key("nested/group"), std::runtime_error);
  }
}
