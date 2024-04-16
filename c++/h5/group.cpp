// Copyright (c) 2019-2022 Simons Foundation
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
// Authors: Henri Menke, Olivier Parcollet, Nils Wentzell

#include "./group.hpp"

#include <hdf5.h>
#include <hdf5_hl.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace h5 {

  group::group(file f) : object(), parent_file(f) {
    id = H5Gopen2(f, "/", H5P_DEFAULT);
    if (id < 0) throw std::runtime_error("Error in h5::group: Opening the root group / for the file " + f.name() + " failed");
  }

  // same function is used in h5::file
  std::string group::name() const {
    // get length of the name
    ssize_t size = H5Iget_name(id, nullptr, 1);

    // reserve a buffer and get name
    std::vector<char> buf(size + 1, 0x00);
    H5Iget_name(id, buf.data(), size + 1);

    // return string
    std::string res = "";
    res.append(&(buf.front()));
    return res;
  }

  bool group::has_key(std::string const &key) const { return H5Lexists(id, key.c_str(), H5P_DEFAULT); }

  bool group::has_subgroup(std::string const &key) const {
    // check if a link with the given name exists
    if (!has_key(key)) return false;

    // check if the link can be opened
    hid_t id_node = H5Oopen(id, key.c_str(), H5P_DEFAULT);
    if (id_node <= 0) return false;

    // check if the opened link is actually a group
    bool r = (H5Iget_type(id_node) == H5I_GROUP);
    H5Oclose(id_node);
    return r;
  }

  bool group::has_dataset(std::string const &key) const {
    // check if a link with the given name exists
    if (!has_key(key)) return false;

    // check if the link can be opened
    hid_t id_node = H5Oopen(id, key.c_str(), H5P_DEFAULT);
    if (id_node <= 0) return false;

    // check if the opened link is actually a dataset
    bool r = (H5Iget_type(id_node) == H5I_DATASET);
    H5Oclose(id_node);
    return r;
  }

  void group::unlink(std::string const &key, bool error_if_absent) const {
    // check if a link with the given name exists
    if (!has_key(key)) {
      // throw an exception if `error_if_absent` is true
      if (error_if_absent) throw std::runtime_error("Error in h5::group: " + key + " does not exist in the group " + name());
      return;
    }

    // remove the link from the group
    auto err = H5Ldelete(id, key.c_str(), H5P_DEFAULT);
    if (err < 0) throw std::runtime_error("Error in h5::group: Unlinking " + key + " in the group " + name() + " failed");
  }

  group group::open_group(std::string const &key) const {
    // return the current group if the key is empty
    if (key.empty()) return *this;

    // check if a link with the key exists
    if (!has_key(key)) throw std::runtime_error("Error in h5::group: " + key + " does not exist in the group " + name());

    // open the subgroup
    object obj = H5Gopen2(id, key.c_str(), H5P_DEFAULT);
    if (obj < 0) throw std::runtime_error("Error in h5::group: Opening the subgroup " + key + " in the group " + name() + " failed");
    return {obj, parent_file};
  }

  group group::create_group(std::string const &key, bool delete_if_exists) const {
    // return the current group if the key is empty
    if (key.empty()) return *this;

    // unlink existing group if 'delete_if_exists' is true
    if (delete_if_exists) unlink(key);

    // create new subgroup
    object obj = H5Gcreate2(id, key.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (not obj.is_valid()) throw std::runtime_error("Error in h5::group: Creating the subgroup " + key + " in the group " + name() + " failed");
    return {obj, parent_file};
  }

  void group::create_softlink(std::string const &target_key, std::string const &key, bool delete_if_exists) const {
    // do nothing if the key or target key is empty
    if (target_key.empty() || key.empty()) return;

    // check if target exists
    if (!has_key(target_key)) throw std::runtime_error("Error in h5::group: " + target_key + " does not exist in the group " + name());

    // unlink existing link or throw exception
    if (delete_if_exists)
      unlink(key, false);
    else if (has_key(key))
      throw std::runtime_error("Error in h5::group: " + key + " already exists in the group " + name());

    // create softlink
    auto const err = H5Lcreate_soft(target_key.c_str(), id, key.c_str(), H5P_DEFAULT, H5P_DEFAULT);
    if (err < 0) throw std::runtime_error("Error in h5::group: Creating the softlink " + key + " -> " + target_key + " failed");
  }

  dataset group::open_dataset(std::string const &key) const {
    // check if a link with the given name exists
    if (!has_key(key)) throw std::runtime_error("Error in h5::group: " + key + " does not exist in the group " + name());

    // open the dataset
    dataset ds = H5Dopen2(id, key.c_str(), H5P_DEFAULT);
    if (!ds.is_valid()) throw std::runtime_error("Error in h5::group: Opening the dataset " + key + " in the group " + name() + " failed");
    return ds;
  }

  dataset group::create_dataset(std::string const &key, datatype ty, dataspace sp, hid_t pl) const {
    // unlink existing link
    unlink(key);

    // create new dataset
    dataset ds = H5Dcreate2(id, key.c_str(), ty, sp, H5P_DEFAULT, pl, H5P_DEFAULT);
    if (!ds.is_valid()) throw std::runtime_error("Error in h5::group: Creating the dataset " + key + " in the group " + name() + " failed");
    return ds;
  }

  dataset group::create_dataset(std::string const &key, datatype ty, dataspace sp) const { return create_dataset(key, ty, sp, H5P_DEFAULT); }

  // C callbacks for the next functions using H5Literate
  extern "C" {

  herr_t get_group_elements_name_ds(::hid_t loc_id, const char *name, const H5L_info_t *, void *opdata) {
    H5O_info_t object_info;
    herr_t err = H5Oget_info_by_name(loc_id, name, &object_info, H5P_DEFAULT);
    if (err < 0) throw std::runtime_error("Error in h5::get_group_elements_name_ds: H5Oget_info_by_name call failed");
    if (object_info.type == H5O_TYPE_DATASET) static_cast<std::vector<std::string> *>(opdata)->push_back(name);
    return 0;
  }

  herr_t get_group_elements_name_grp(::hid_t loc_id, const char *name, const H5L_info_t *, void *opdata) {
    H5O_info_t object_info;
    herr_t err = H5Oget_info_by_name(loc_id, name, &object_info, H5P_DEFAULT);
    if (err < 0) throw std::runtime_error("Error in h5::get_group_elements_name_grp: H5Oget_info_by_name call failed");
    if (object_info.type == H5O_TYPE_GROUP) static_cast<std::vector<std::string> *>(opdata)->push_back(name);
    return 0;
  }

  herr_t get_group_elements_name_ds_grp(::hid_t loc_id, const char *name, const H5L_info_t *, void *opdata) {
    H5O_info_t object_info;
    herr_t err = H5Oget_info_by_name(loc_id, name, &object_info, H5P_DEFAULT);
    if (err < 0) throw std::runtime_error("Error in h5::get_group_elements_name_ds_grp: H5Oget_info_by_name call failed");
    if ((object_info.type == H5O_TYPE_GROUP) or (object_info.type == H5O_TYPE_DATASET))
      static_cast<std::vector<std::string> *>(opdata)->push_back(name);
    return 0;
  }

  } // extern "C"

  std::vector<std::string> group::get_all_subgroup_names() const {
    std::vector<std::string> grp_name;
    int r = H5Literate(::hid_t(id), H5_INDEX_NAME, H5_ITER_NATIVE, nullptr, get_group_elements_name_grp, static_cast<void *>(&grp_name));
    if (r != 0) throw std::runtime_error("Error in h5::group: Iterating over subgroups of the group " + name() + "failed");
    return grp_name;
  }

  std::vector<std::string> group::get_all_dataset_names() const {
    std::vector<std::string> ds_name;
    int r = H5Literate(::hid_t(id), H5_INDEX_NAME, H5_ITER_NATIVE, nullptr, get_group_elements_name_ds, static_cast<void *>(&ds_name));
    if (r != 0) throw std::runtime_error("Error in h5::group: Iterating over datasets of the group " + name() + "failed");
    return ds_name;
  }

  std::vector<std::string> group::get_all_subgroup_dataset_names() const {
    std::vector<std::string> ds_name;
    int r = H5Literate(::hid_t(id), H5_INDEX_NAME, H5_ITER_NATIVE, nullptr, get_group_elements_name_ds_grp, static_cast<void *>(&ds_name));
    if (r != 0) throw std::runtime_error("Error in h5::group: Iterating over datasets and subgroups of the group " + name() + "failed");
    return ds_name;
  }

} // namespace h5
