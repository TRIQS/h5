/*******************************************************************************
 *
 * TRIQS: a Toolbox for Research in Interacting Quantum Systems
 *
 * Copyright (C) 2018 by N. Wentzell
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

#include "./../nda/test_common.hpp"

#include <h5/h5.hpp>

TEST(H5, ComplexBkwd) {

  {
    h5::file file("complex_old.h5", 'w');
    h5::group top(file);

    auto g = top.create_group("cplx");

    double r = 1.0, i = 2.0;
    h5_write(g, "r", r);
    h5_write(g, "i", i);
  }

  {
    h5::file file("complex_old.h5", 'r');

    std::complex<double> c;
    h5_read(file, "cplx", c);

    dcomplex exact = {1.0, 2.0};
    EXPECT_EQ(c, exact);
  }
};

MAKE_MAIN;
