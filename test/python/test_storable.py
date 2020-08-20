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

from storable import Storable
from h5 import HDFArchive

class TestStorable(unittest.TestCase):

    def test_storable(self):

        obj = Storable()
        obj.vec = [1, 2, 4, 5]
        obj.s = "some other string"

        with HDFArchive('h5_class.h5','w') as arch:
            arch['obj'] = obj

        with HDFArchive('h5_class.h5','r') as arch:
            obj_in = arch['obj']

        self.assertTrue(all(np.array(obj.vec)== np.array(obj_in.vec)))
        self.assertEqual(obj.s, obj_in.s)

if __name__ == '__main__':
    unittest.main()
