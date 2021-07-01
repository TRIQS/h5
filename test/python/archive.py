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

        # === Read from disk
        with HDFArchive('hdf_archive2.h5','r') as arch:
            check_archive_contents(arch)
            as_bytes_from_memory = arch.as_bytes()

        # === Read from disk as bytes and create file in memory
        with open('hdf_archive2.h5', 'rb') as bytestream:
            as_bytes_from_disk = bytestream.read()
            self.assertEqual(as_bytes_from_memory, as_bytes_from_disk)
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

if __name__ == '__main__':
    unittest.main()
