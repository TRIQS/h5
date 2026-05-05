"""Tests reading complex datasets written by external libraries (Julia HDF5.jl, h5py)
that store complex values as an HDF5 compound type {r, i} rather than the TRIQS
trailing-2-dim convention."""

import unittest
import numpy as np

from h5 import HDFArchive


def _check(testcase, filename):
    with HDFArchive(filename, 'r') as h:
        g = h['group']

        scalar = g['scalar_complex']
        testcase.assertEqual(complex(scalar), 1.0 + 2.0j)

        vec = g['vector_complex']
        testcase.assertEqual(vec.dtype, np.complex128)
        np.testing.assert_array_equal(vec, np.array([1+2j, 3+4j, 5+6j], dtype=np.complex128))

        # Both reference files store a (2, 3) row-major matrix with m[r, c] = r + 10*c
        # (and m == conj for the complex variant). See test/c++/gen_julia_complex.jl
        # for why Julia's matrix is permutedims'd before writing.
        expected_cplx = np.array([[0+0j, 10+10j, 20+20j],
                                  [1+1j, 11+11j, 21+21j]], dtype=np.complex128)
        mat = g['matrix_complex']
        testcase.assertEqual(mat.dtype, np.complex128)
        np.testing.assert_array_equal(mat, expected_cplx)

        expected_flt = np.array([[0.0, 10.0, 20.0],
                                 [1.0, 11.0, 21.0]], dtype=np.float64)
        flt = g['dataset_float']
        testcase.assertEqual(flt.dtype, np.float64)
        np.testing.assert_array_equal(flt, expected_flt)


class TestExternalComplex(unittest.TestCase):
    def test_julia(self):
        _check(self, 'julia_complex.ref.h5')

    def test_h5py(self):
        _check(self, 'h5py_complex.ref.h5')


if __name__ == '__main__':
    unittest.main()
