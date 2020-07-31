// Copyright (c) 2019-2020 Simons Foundation
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

#pragma once
#include <utility>

#include "./file.hpp"

namespace h5 {

  /**
   *  HDF5 group
   */
  class group : public object {

    file parent_file;

    public:
    group() = default; // for python converter only

    /// Takes the "/" group at the top of the file
    group(file f);

    ///
    group(group const &) = default;

    private:
    // construct from the bare object and the parent
    // internal use only for open/create subgroup
    group(object obj, file _parent_file) : object{obj}, parent_file(std::move(_parent_file)) {}

    public:
    /// Name of the group
    [[nodiscard]] std::string name() const;

    /// Access to the parent file
    [[nodiscard]] file get_file() const { return parent_file; }

    /**
     * True iff key is an object in the group
     *
     * @param key
     */
    [[nodiscard]] bool has_key(std::string const &key) const;

    /**
     * True iff key is a subgroup of this.
     *
     * @param key
     */
    [[nodiscard]] bool has_subgroup(std::string const &key) const;

    /**
     * True iff key is a dataset of this.
     *
     * @param key
     */
    [[nodiscard]] bool has_dataset(std::string const &key) const;

    /**
     * Unlinks the subgroup key if it exists
     * No error is thrown if key does not exists
     * NB : unlink is almost a remove, but it does not really remove from the file (memory is not freed).
     * After unlinking a large object, a h5repack may be needed. Cf HDF5 documentation.
     *
     * @param key The name of the subgroup to be removed.
     * @param error_if_absent If True, throws an error if the key is missing.
     */
    void unlink(std::string const &key, bool error_if_absent = false) const;

    /**
     * Open a subgroup.
     * Throws std::runtime_error if it does not exist.
     *
     * @param key  The name of the subgroup. If empty, return this group
     */
    [[nodiscard]] group open_group(std::string const &key) const;

    /**
     * Create a subgroup in this group
     * 
     * @param key  The name of the subgroup. If empty, return this group.
     * @param delete_if_exists  Unlink the group if it exists
     */
    group create_group(std::string const &key, bool delete_if_exists = true) const; // NOLINT

    /**
     * Open a existing DataSet in the group.
     * Throws std::runtime_error if it does not exist.
     *
     * @param key  The name of the subgroup. If empty, return this group
     */
    [[nodiscard]] dataset open_dataset(std::string const &key) const;

    /**
     * Create a dataset in this group
     * 
     * @param key  The name of the dataset. 
     * @param ty Datatype
     * @param sp Dataspace
     */
    [[nodiscard]] dataset create_dataset(std::string const &key, datatype ty, dataspace sp) const;

    /**
     * Create a dataset in this group
     * 
     * @param key  The name of the dataset. 
     * @param ty Datatype
     * @param sp Dataspace
     * @param pl Property list
     */
    [[nodiscard]] dataset create_dataset(std::string const &key, datatype ty, dataspace sp, hid_t pl) const;

    /// Returns all names of subgroup of  G
    [[nodiscard]] std::vector<std::string> get_all_subgroup_names() const;

    /// Returns all names of dataset of G
    [[nodiscard]] std::vector<std::string> get_all_dataset_names() const;

    /// Returns all names of dataset of G
    [[nodiscard]] std::vector<std::string> get_all_subgroup_dataset_names() const;
  };

} // namespace h5
