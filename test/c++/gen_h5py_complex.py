import h5py
import numpy as np

with h5py.File("h5py_complex.ref.h5", "w") as f:
    g = f.create_group("group")
    g["scalar_complex"] = np.complex128(1.0 + 2.0j)
    g["vector_complex"] = np.array([1 + 2j, 3 + 4j, 5 + 6j], dtype=np.complex128)
    i, j = np.indices((2, 3))
    g["matrix_complex"] = (i + 10 * j + 1j * (i + 10 * j)).astype(np.complex128)
    g["dataset_float"]  = (i + 10 * j).astype(np.float64)
