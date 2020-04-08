################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2020 Simons Foundation
#   author: N. Wentzell
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

from h5 import HDFArchive

class TestIssueMultifile(unittest.TestCase):

    def test_issue_multifile(self):

        # Open a file more than once and write to it
        with HDFArchive("multifile.h5", 'w') as arch:
            arch['i'] = 14

        with HDFArchive("multifile.h5", 'a') as arch:
            arch['d'] = 3.2

        # Open a file more than once and read from it
        with HDFArchive("multifile.h5", 'r') as arch:
            self.assertEqual(arch['i'], 14)
            self.assertEqual(arch['d'], 3.2)


        # Open a file more than once and write to it
        with HDFArchive("multifile.h5", 'a') as arch:
            arch['c'] = 1.2 + 3j

        with HDFArchive("multifile.h5", 'a') as arch:
            arch['s'] = "a string"

        # Open a file more than once and read from it
        with HDFArchive("multifile.h5", 'r') as arch:
            self.assertEqual(arch['c'], 1.2 + 3j)
            self.assertEqual(arch['s'], "a string")


if __name__ == '__main__':
    unittest.main()
