/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2011 by O. Parcollet, N. Wentzell
 *
 * TRIQS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * TRIQS. If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

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
