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
#include <array>
#include <string>

TEST(H5, Array) {

  // write
  auto arr_str = std::array<std::string, 2>{"a", "abc"};
  auto arr_dbl = std::array<double, 2>{1.0, 2.0};

  {
    h5::file file{"test_arr.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "arr_str", arr_str);
    h5_write(grp, "arr_dbl", arr_dbl);
  }

  // read
  std::array<std::string, 2> arr_str_r;
  std::array<double, 2> arr_dbl_r;

  {
    h5::file file{"test_arr.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "arr_str", arr_str_r);
    h5_read(grp, "arr_dbl", arr_dbl_r);
  }

  // compare
  EXPECT_EQ(arr_str, arr_str_r);
  EXPECT_EQ(arr_dbl, arr_dbl_r);
}
