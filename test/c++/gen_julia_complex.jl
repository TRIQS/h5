using HDF5

# Julia is column-major while HDF5 stores data row-major. HDF5.jl avoids a copy
# on write by emitting the in-memory bytes as-is and reversing the dimension list
# in the file metadata. As a consequence, a Julia (M, N) matrix lands in the file
# as shape (N, M) and reading it row-major in C/Python yields its transpose.
#
# To produce the same on-disk byte layout as h5py (which writes a (2, 3) matrix
# row-major), we permute dimensions before writing so HDF5.jl's reversal lands
# on shape (2, 3) with row-major rows [0, 10, 20] and [1, 11, 21].
h5open("julia_complex.ref.h5", "w") do f
    g = create_group(f, "group")
    g["scalar_complex"] = ComplexF64(1.0 + 2.0im)
    g["vector_complex"] = ComplexF64[1+2im, 3+4im, 5+6im]

    mat_c = ComplexF64[i + 10*j + (i + 10*j)*im for i in 0:1, j in 0:2]
    mat_f = Float64[i + 10*j for i in 0:1, j in 0:2]
    g["matrix_complex"] = permutedims(mat_c, (2, 1))
    g["dataset_float"]  = permutedims(mat_f, (2, 1))
end
