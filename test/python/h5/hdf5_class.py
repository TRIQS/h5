################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
#
# TRIQS is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# TRIQS is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TRIQS. If not, see <http://www.gnu.org/licenses/>.
#
################################################################################
import unittest
import numpy as np


from storable import Storable
from h5 import HDFArchive

class TestHdf5Class(unittest.TestCase):

    def test_hdf5_class(self):

        obj = Storable()
        obj.vec = [1, 2, 4, 5]
        obj.s = "some other string"

        with HDFArchive('hdf5_class.h5','w') as arch:
            arch['obj'] = obj

        with HDFArchive('hdf5_class.h5','r') as arch:
            obj_in = arch['obj']

        self.assertEqual(obj.vec, obj_in.vec)
        self.assertEqual(obj.s, obj_in.s)

if __name__ == '__main__':
    unittest.main()
