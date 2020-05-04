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
#include <optional>

TEST(H5, Optional) {

  std::optional<int> a(18), b;

  if (a) std::cout << *a << std::endl;
  {
    h5::file file("test_opt.h5", 'w');
    h5_write(file, "A", a);
  }
  {
    h5::file file("test_opt.h5", 'r');
    h5_read(file, "A", b);
  }

  EXPECT_TRUE(bool(b));
  EXPECT_EQ(a, b);
}
