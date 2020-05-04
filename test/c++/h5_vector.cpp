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
#include <variant>

TEST(H5, Vector) {

  // write
  std::vector<std::string> m = {"a", "b"};
  std::vector<double> mv     = {1.0, 2.0};

  {
    h5::file file{"test_vec.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "vec_str", m);
    h5_write(grp, "vec_dbl", mv);
  }

  // read
  std::vector<std::string> mm;
  std::vector<double> mmv;

  {
    h5::file file{"test_vec.h5", 'r'};
    h5::group grp{file};
    h5_read(grp, "vec_str", mm);
    h5_read(grp, "vec_dbl", mmv);
  }

  // compare
  EXPECT_EQ(m, mm);
  EXPECT_EQ(mv, mmv);
}

TEST(H5, VectorAttribute) {
  // write
  std::vector<std::vector<std::string>> vvs  = {{"a", "b"}, {"c", "d"}, {"e", "f"}};
  std::vector<std::vector<std::string>> evvs = {};
  std::vector<std::vector<std::string>> vevs = {{}, {}, {}};
  std::vector<std::vector<std::string>> vves = {{"", ""}, {"", ""}, {"", ""}};

  {
    h5::file file{"test_attribute.h5", 'w'};
    h5::group grp{file};
    h5_write(grp, "vec", 0);

    auto ds = grp.open_dataset("vec");
    h5_write_attribute(ds, "attr_vvs", vvs);
    h5_write_attribute(ds, "attr_evvs", evvs);
    h5_write_attribute(ds, "attr_vevs", vevs);
    h5_write_attribute(ds, "attr_vves", vves);
  }

  // read
  std::vector<std::vector<std::string>> rvvs, revvs, rvevs, rvves;

  {
    h5::file file{"test_attribute.h5", 'r'};
    h5::group grp{file};

    auto ds = grp.open_dataset("vec");
    h5_read_attribute(ds, "attr_vvs", rvvs);
    h5_read_attribute(ds, "attr_evvs", revvs);
    h5_read_attribute(ds, "attr_vevs", rvevs);
    h5_read_attribute(ds, "attr_vves", rvves);
  }

  // compare
  EXPECT_EQ(vvs, rvvs);
  EXPECT_EQ(evvs, revvs);
  EXPECT_EQ(vevs, rvevs);
  EXPECT_EQ(vves, rvves);
}
