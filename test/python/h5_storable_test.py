import unittest
import numpy as np
import pickle

from h5_storable import Storable
from h5 import HDFArchive

class TestStorable(unittest.TestCase):

    def test_h5(self):
        obj = Storable()
        with HDFArchive("cls_ess.h5", 'w') as h:
            h['obj'] = obj

        with HDFArchive("cls_ess.h5", 'r') as h:
            obj2 = h['obj']
        self.assertTrue(all(np.array(obj.vec)== np.array(obj2.vec)))
        self.assertEqual(obj.s, obj2.s)

    def test_pickle(self):

        obj = Storable()
        obj.vec = [1, 2, 4, 5]
        obj.s = "some other string"

        s = pickle.dumps(obj)
        obj_in = pickle.loads(s)

        self.assertTrue(all(np.array(obj.vec)== np.array(obj_in.vec)))
        self.assertEqual(obj.s, obj_in.s)


if __name__ == '__main__':
    unittest.main()
