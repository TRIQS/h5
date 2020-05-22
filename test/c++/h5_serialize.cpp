/*******************************************************************************
 *
 * H5: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2020 Simons Foundation
 *    author: N. Wentzell
 *
 * H5 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * H5 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * H5. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "./test_common.hpp"

#include <h5/h5.hpp>
#include <h5/serialization.hpp>

#include <array>

TEST(H5, Serialize) {

  auto arr_str = std::array<std::string, 2>{"a", "abc"};
  auto arr_dbl = std::array<double, 2>{1.0, 2.0};

  // serialize then deserialize
  auto arr_str_ser = h5::deserialize<std::array<std::string, 2>>(h5::serialize(arr_str));
  auto arr_dbl_ser = h5::deserialize<std::array<double, 2>>(h5::serialize(arr_dbl));

  // compare
  EXPECT_EQ(arr_str, arr_str_ser);
  EXPECT_EQ(arr_dbl, arr_dbl_ser);
}
