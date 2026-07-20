# Copyright (c) 2019-2020 Simons Foundation
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
from math import isnan

from h5 import HDFArchive
from h5.formats import register_class

class OrderReliantReduce:
    """A __reduce_to_dict__/__factory_from_dict__ class (not a plain dict) whose
    reconstruction depends on the stored key order, used to check that key
    ordering is preserved for any reduce-based type, not only dict."""
    def __init__(self, order):
        self.order = list(order)
    def __reduce_to_dict__(self):
        return {k: i for i, k in enumerate(self.order)}
    @classmethod
    def __factory_from_dict__(cls, name, D):
        return cls(D.keys())

register_class(OrderReliantReduce)

def assert_arrays_are_close(a, b, precision = 1.e-6):
    d = np.amax(np.abs(a - b))
    assert  d< precision, "Arrays are different. Difference is %s.\n %s \n\n --------- \n\n %s"%(d,a,b)

def assert_array_close_to_scalar(a, x, precision = 1.e-6):
    assert_arrays_are_close(a, np.identity(a.shape[0])*(x), precision)


class TestHdf5Io(unittest.TestCase):

    def test_archive_base(self):
        d = {'dbl' : 1.0, 'lst' : [1,[1],'a']}
        
        # === Write to archive
        with HDFArchive('hdf_archive2.h5','w', init = list(d.items())) as arch:
        
            arch['int'] = 100
            arch['arr'] = np.array([[1,2,3],[4,5,6]])
            arch['tpl'] = (2,[2],'b')
            arch['dct'] = { 'a':[10], 'b':20 }
            arch['nan'] = float('nan')
            arch['nanarr'] = np.array([np.nan, 1.0])
        
            arch.create_group('grp')
            grp = arch['grp']
            grp['int'] = 98
            grp['tpl'] = (3,[3],'c')
            grp['dct'] = { 'a':[30], 'b':40 }
        
        # === Read access
        with HDFArchive('hdf_archive2.h5','r') as arch:  
            dct = arch['dct']
            grp = arch['grp']
        
        # === Read/Write access
        with HDFArchive('hdf_archive2.h5','a') as arch:
        
            dct = arch['dct']
            dct['c'] = 'triqs'
            arch['dct'] = dct
        
            grp = arch['grp']
            dct = grp['dct']
            dct['c'] = 'qmc'
        
            grp['dct'] = dct
            grp['d'] = 700
            grp['x'] = 1.5
            grp['y'] = 'zzz'
        
        
        # === Final checks
        def check_archive_contents(arch):
            self.assertEqual( arch['dbl'] , 1.0 )
            
            self.assertEqual( arch['lst'] , [1,[1],'a'] )
            self.assertEqual( arch['int'] , 100 )
            assert_arrays_are_close( arch['arr'] , np.array([[1, 2, 3], [4, 5, 6]]) )
            self.assertEqual( arch['tpl'] , (2,[2],'b') )
            self.assertEqual( arch['dct'] , {'a':[10], 'b':20, 'c':'triqs'} )
            self.assertTrue( isnan(arch['nan']) )
            self.assertTrue( np.array_equal(np.isnan(arch['nanarr']), np.array([True, False])) )

            self.assertEqual( arch['grp']['int'] , 98 )
            self.assertEqual( arch['grp']['tpl'] , (3,[3],'c') )
            self.assertEqual( arch['grp']['dct'] , { 'a':[30], 'b':40, 'c':'qmc'} )
            self.assertEqual( arch['grp']['d'] , 700 )
            self.assertEqual( arch['grp']['x'] , 1.5 )
            self.assertEqual( arch['grp']['y'] , 'zzz' )

        # === Read from disk and check contents
        with HDFArchive('hdf_archive2.h5','r') as arch:
            check_archive_contents(arch)
            as_bytes_from_memory = arch.as_bytes()

        # === Check contents of file created from byte buffer
        with HDFArchive(as_bytes_from_memory) as arch:
            check_archive_contents(arch)

        # === Read from disk as bytes and create file in memory
        with open('hdf_archive2.h5', 'rb') as bytestream:
            as_bytes_from_disk = bytestream.read()
            with HDFArchive(as_bytes_from_disk) as arch:
                check_archive_contents(arch)

    def test_hdf5_bool(self):

        # Write
        with HDFArchive('bool.h5','w') as arch:
            arch['t'] = True
            arch['f'] = False
            arch['i'] = 10

        with HDFArchive('bool.h5','r') as arch:
            t = arch['t']
            f = arch['f']
            i = arch['i']

        self.assertTrue(t)
        self.assertFalse(f)
        self.assertIs(type(t),bool)
        self.assertIs(type(f),bool)
        self.assertIs(type(i),int)

    def test_hdf5_types(self):
        filename = 'h5archive.h5'

        p = dict(
            my_flag=True,
            my_int=1,
            my_long=1,
            my_float=1.,
            my_complex=1.j,
            my_string='foobar',
            my_string_unicode='foobar',
            my_ndarray_int=np.array([1]),
            my_ndarray_float=np.array([1.]),
            my_ndarray_complex=np.array([1.j]),
            )

        with HDFArchive(filename, 'w') as a:
            a['p'] = p

        with HDFArchive(filename, 'r') as a:
            p_ref = a['p']

        for key in list(p.keys()):
            print(key, type(p[key]), type(p_ref[key]))
            assert( type(p[key]) == type(p_ref[key]) )

            if type(p[key]) == np.ndarray:
                assert( p[key].dtype == p_ref[key].dtype )
                print('dtypes: ', p[key].dtype, p_ref[key].dtype)

    def test_hdf5_types(self):
        filename = 'h5archive.h5'

        # Test creation of softlinks
        with HDFArchive(filename, 'w') as a:
            a.create_group("group")
            a['data'] = 11
            a['group']['data'] = 22
            a.create_softlink('data', 'link')
            a.create_softlink('/group/data', '/group/link')

        with HDFArchive(filename, 'r') as a:
            link_data = a['link']
            link_group_data = a['group']['link']

        self.assertEqual(link_data, 11)
        self.assertEqual(link_group_data, 22)

        # Test overwriting of softlinks
        with HDFArchive(filename, 'a') as a:
            a['other data'] = 33
            a.create_softlink('other data', 'link')

        with HDFArchive(filename, 'r') as a:
            link_data = a['link']

        self.assertEqual(link_data, 33)

        # Test exception on overwriting of softlinks
        with HDFArchive(filename, 'a') as a:
            with self.assertRaises(RuntimeError) :
                a.create_softlink('data', 'link', delete_if_exists = False)

    def _assert_dict_restored(self, actual, expected):
        """Fail unless `actual` reproduces `expected` exactly: identical key
        order, identical key types, and identical value types/values
        (recursing into nested dicts, comparing arrays elementwise)."""
        self.assertIsInstance(actual, dict)
        # 1. same ordering (and same key values)
        self.assertEqual(list(actual.keys()), list(expected.keys()))
        # 2. same key types
        for ka, ke in zip(actual.keys(), expected.keys()):
            self.assertIs(type(ka), type(ke), "key %r vs %r: type mismatch"%(ka, ke))
        # 3. same value types/values
        for k in expected:
            va, ve = actual[k], expected[k]
            if isinstance(ve, np.ndarray):
                self.assertIsInstance(va, np.ndarray)
                assert_arrays_are_close(va, ve)
            elif isinstance(ve, dict):
                self._assert_dict_restored(va, ve)
            else:
                self.assertIs(type(va), type(ve), "value for key %r: type mismatch"%(k,))
                self.assertEqual(va, ve)

    def _roundtrip(self, filename, obj):
        with HDFArchive(filename, 'w') as a:
            a['d'] = obj
        with HDFArchive(filename, 'r') as a:
            return a['d']

    def test_dict_ordering(self):
        # insertion order differs from lexicographic order
        d = {'c': 1, 'a': 2, 'b': 3, 'delta': 4}
        self.assertNotEqual(list(d.keys()), sorted(d.keys()))
        self._assert_dict_restored(self._roundtrip('h5_dict_ordering.h5', d), d)

    def test_dict_value_types(self):
        # heterogeneous value types under (deliberately unsorted) string keys
        d = {
            'i': 5,
            'f': 2.5,
            's': 'text',
            'b': True,
            'lst': [1, 'a', 2.5],
            'nested': {'x': 1, 'y': 2},
            'arr': np.array([1.0, 2.0, 3.0]),
        }
        self._assert_dict_restored(self._roundtrip('h5_dict_valtypes.h5', d), d)

    def test_dict_nested_ordering(self):
        # order must be preserved recursively for nested dicts
        d = {'z': {'q': 1, 'a': 2}, 'y': 3, 'x': {'m': 4, 'b': 5}}
        self._assert_dict_restored(self._roundtrip('h5_dict_nested.h5', d), d)

    def test_reduce_class_key_order(self):
        # key order is preserved for any __reduce_to_dict__ class, not only dict
        obj = OrderReliantReduce(['z', 'a', 'm', 'b'])
        r = self._roundtrip('h5_reduce_order.h5', obj)
        self.assertIsInstance(r, OrderReliantReduce)
        self.assertEqual(r.order, obj.order)

    def test_dict_non_string_keys_raise(self):
        # Dict storage is limited to string keys: non-string keys must fail
        # loudly rather than be silently stringified (which loses the key type
        # and collapses collisions like 1 vs '1').
        bad = [
            {1: 'a'},              # int
            {1.5: 'a'},            # float
            {(1, 2): 'a'},         # tuple
            {None: 'a'},           # None
            {True: 'a'},           # bool
            {'ok': 1, 2: 'b'},     # mixed string + non-string
        ]
        for d in bad:
            with HDFArchive('h5_dict_badkeys.h5', 'w') as a:
                with self.assertRaises(TypeError):
                    a['d'] = d

    def test_same_session_dataset_readback(self):
        # Writing a dataset (scalar or ndarray) must update the key cache so the
        # entry is visible for keys()/'in'/read within the same open session,
        # not only after closing and reopening the file.
        with HDFArchive('same_session.h5', 'w') as arch:
            arch['x'] = 1.0
            arch['arr'] = np.array([1, 2, 3])

            self.assertIn('x', arch)
            self.assertIn('arr', arch)
            self.assertEqual(sorted(arch.keys()), ['arr', 'x'])
            self.assertEqual(arch['x'], 1.0)
            assert_arrays_are_close(arch['arr'], np.array([1, 2, 3]))

            # overwriting an existing dataset must not duplicate its key
            arch['x'] = 2.0
            self.assertEqual(arch['x'], 2.0)
            self.assertEqual(sorted(arch.keys()).count('x'), 1)

        # contents remain correct after reopening
        with HDFArchive('same_session.h5', 'r') as arch:
            self.assertEqual(sorted(arch.keys()), ['arr', 'x'])
            self.assertEqual(arch['x'], 2.0)

    def test_in_memory_dataset_readback(self):
        # Same-session readback for an in-memory archive (descriptor=None).
        arch = HDFArchive()
        arch['s'] = 42
        arch['v'] = np.array([1.0, 2.0])
        self.assertIn('s', arch)
        self.assertEqual(arch['s'], 42)
        assert_arrays_are_close(arch['v'], np.array([1.0, 2.0]))
        self.assertEqual(sorted(arch.keys()), ['s', 'v'])

    def test_delitem_persists_to_disk(self):
        # Deleting a key must unlink it from the HDF5 file, not merely drop it
        # from the in-memory cache; the deletion must survive a reopen. Covers
        # both a dataset and a subgroup (TRIQS/h5 issue #14).
        with HDFArchive('h5_delitem.h5', 'w') as a:
            a['dset'] = np.array([1, 2])
            a['grp'] = {'k': 1}
            a['keep'] = 0
        with HDFArchive('h5_delitem.h5', 'a') as a:
            del a['dset']       # delete a dataset
            del a['grp']        # delete a subgroup
            self.assertEqual(list(a.keys()), ['keep'])
        with HDFArchive('h5_delitem.h5', 'r') as a:
            self.assertEqual(sorted(a.keys()), ['keep'])

if __name__ == '__main__':
    unittest.main()
