# Copyright (c) 2018-2020 Simons Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


r"""
Python interface to the h5 library.

This package exposes a dict-like API for reading and writing HDF5 files:

* :class:`HDFArchive` — open an HDF5 file (or in-memory buffer) for
  reading/writing. Supports the modes ``'r'``, ``'w'``, ``'a'``, ``'r+'``.
* :class:`HDFArchiveGroup` — a view onto a subgroup of an
  :class:`HDFArchive`. Same dict-like interface, scoped to that subgroup.
* :class:`HDFArchiveInert` — a no-op stand-in used in MPI contexts so that
  non-root ranks can execute archive code without performing any I/O.

Custom Python classes can be made archive-compatible by registering them
with :func:`h5.formats.register_class` and providing
``__reduce_to_dict__`` / ``__factory_from_dict__`` (or ``__write_hdf5__``)
methods.
"""

from .archive import HDFArchive, HDFArchiveGroup, HDFArchiveInert
__all__ = ['HDFArchive', 'HDFArchiveGroup', 'HDFArchiveInert']
