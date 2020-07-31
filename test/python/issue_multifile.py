# Copyright (c) 2020 Simons Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
