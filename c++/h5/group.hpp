// Copyright (c) 2019-2021 Simons Foundation
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

/**
 * @file
 * @brief Provides a handle to an HDF5 group and various methods to simplify the creation/opening of
 * subgroups, datasets and softlinks within a group.
 */

#ifndef LIBH5_GROUP_HPP
#define LIBH5_GROUP_HPP

#include "./file.hpp"

#include <string>
#include <utility>
#include <vector>

namespace h5 {

  /**
   * @ingroup data_model
   * @brief A handle to an HDF5 group.
   *
   * @details This class inherits from the general h5::object class. Each group stores the parent h5::file
   * to which it belongs.
   *
   * It provides various methods to simplify the creation of new and opening of existing groups, subgroups,
   * datasets and softlinks within the current group.
   */
  class group : public object {
    // File to which the group belongs.
    file parent_file;

    public:
    /// Default constructor (only necessary for the Python interface).
    group() = default;

    /**
     * @brief Constructor to open the root ("/") group in the given file.
     * @param f h5::file.
     */
    group(file f);

    private:
    // Copy the given object and parent file (only used internally to open/create a subgroup).
    group(object obj, file _parent_file) : object{obj}, parent_file{std::move(_parent_file)} {}

    public:
    /// Get the name of the group.
    [[nodiscard]] std::string name() const;

    /// Get the parent file to which the group belongs.
    [[nodiscard]] file get_file() const { return parent_file; }

    /**
     * @brief Check if a link with the given key exists in the group.
     *
     * @param key Name of the link.
     * @return True if the link exists, false otherwise.
     */
    [[nodiscard]] bool has_key(std::string const &key) const;

    /**
     * @brief Check if a subgroup with the given key exists in the group and is accessible.
     *
     * @param key Name of the subgroup.
     * @return True if the subgroup exists and can be accessed, false otherwise.
     */
    [[nodiscard]] bool has_subgroup(std::string const &key) const;

    /**
     * @brief Check if a dataset with the given key exists in the group and is accessible.
     *
     * @param key Name of the dataset.
     * @return True if the dataset exists, false otherwise.
     */
    [[nodiscard]] bool has_dataset(std::string const &key) const;

    /**
     * @brief Remove a link with the given key from the group.
     *
     * @details It simply calls `H5Ldelete` to delete the link. If the given link does not exist, it throws
     * an exception if `error_if_absent == true`, otherwise it does nothing.
     *
     * @param key Name of the link to be removed.
     * @param error_if_absent If true, throws an exception if the key is not the name of a link in the group.
     */
    void unlink(std::string const &key, bool error_if_absent = false) const;

    /**
     * @brief Open a subgroup with the given key in the group.
     *
     * @details If the given key is empty, a handle to the current group is returned. Throws an exception if the
     * subgroup fails to be opened.
     *
     * @param key Name of the subgroup.
     * @return A handle to the opened subgroup.
     */
    [[nodiscard]] group open_group(std::string const &key) const;

    /**
     * @brief Create a subgroup with the given key in the group.
     *
     * @details If a subgroup with the given key already exists, it is unlinked first if `delete_if_exists == true`.
     * If the given key is empty, a handle to the current group is returned. Throws an exception if the subgroup fails
     * to be created.
     *
     * @param key Name of the subgroup to be created.
     * @param delete_if_exists If true, unlink first an existing subgroup with the same name.
     * @return A handle to the created subgroup.
     */
    group create_group(std::string const &key, bool delete_if_exists = true) const;

    /**
     * @brief Create a softlink with the given key to a target with a given target key in this group.
     *
     * @details Does nothing if the key or target key is empty. If `delete_if_exists == true`, it first unlinks
     * an existing link with the same name. Throws an exception if the target does not exist, if a link with
     * the given key already exists and `delete_if_exists == false`, or if the softlink fails to be created.
     *
     * @param target_key Name of target.
     * @param key Name of the softlink to be created.
     * @param delete_if_exists If true, unlink first an existing key with the same name.
     */
    void create_softlink(std::string const &target_key, std::string const &key, bool delete_if_exists = true) const;

    /**
     * @brief Open a dataset with the given key in the group.
     *
     * @details Throws an exception if there exists no link with the given key or if the dataset fails to be opened.
     *
     * @param key Name of the dataset.
     * @return A handle to the opened dataset.
     */
    [[nodiscard]] dataset open_dataset(std::string const &key) const;

    /**
     * @brief Create a dataset with the given key, datatype, dataspace and dataset creation property list in this group.
     *
     * @details It first unlinks an existing dataset with the same name. Throws an exception if the dataset fails to be
     * created.
     *
     * @param key Name of the dataset to be created.
     * @param ty h5::datatype.
     * @param sp h5::dataspace.
     * @param pl Dataset creation property list.
     * @return A handle to the created dataset.
     */
    dataset create_dataset(std::string const &key, datatype ty, dataspace sp, hid_t pl) const;

    /**
     * @brief Create a dataset with the given key, datatype and dataspace in this group.
     *
     * @details It simply calls group::create_dateset with the default dataset creation property list.
     *
     * @param key Name of the dataset to be created.
     * @param ty h5::datatype.
     * @param sp h5::dataspace.
     * @return A handle to the created dataset.
     */
    dataset create_dataset(std::string const &key, datatype ty, dataspace sp) const;

    /**
     * @brief Get all the names of the subgroups in the current group.
     * @return A vector with the names of all the subgroups.
     */
    [[nodiscard]] std::vector<std::string> get_all_subgroup_names() const;

    /**
     * @brief Get all the names of the datasets in the current group.
     * @return A vector with the names of all the datasets.
     */
    [[nodiscard]] std::vector<std::string> get_all_dataset_names() const;

    /**
     * @brief Get all the names of the subgroups and datasets in the current group.
     * @return A vector with the names of all the subgroups and datasets.
     */
    [[nodiscard]] std::vector<std::string> get_all_subgroup_dataset_names() const;
  };

} // namespace h5

#endif // LIBH5_GROUP_HPP
