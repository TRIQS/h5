################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011 by M. Ferrero, O. Parcollet
# Copyright (C) 2017 by H. U.R. Strand
# Copyright (C) 2020 Simons Foundation
#    authors: N. Wentzell
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
        
        
        # === Final check
        arch = HDFArchive('hdf_archive2.h5','r')
        
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
