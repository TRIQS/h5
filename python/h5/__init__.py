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
Python interface to the h5 library: a dict-like API for reading and writing HDF5
files.

See :mod:`h5.archive` for the main archive classes and :mod:`h5.formats` for
registering custom classes.

Examples
--------

Values are written and read back by key, and standard Python containers as well
as numpy arrays are supported out of the box:

.. code-block:: python

    import numpy as np
    from h5 import HDFArchive

    # write some data to an archive
    with HDFArchive("example.h5", 'w') as ar:
        ar['x'] = 1.0
        ar['vec'] = np.array([1, 2, 3])
        ar['params'] = {'beta': 10.0, 'n': 5}

    # read it back
    with HDFArchive("example.h5", 'r') as ar:
        print(ar.keys())      # ['params', 'vec', 'x']
        print(ar['x'])        # 1.0
        print(ar['params'])   # {'beta': 10.0, 'n': 5}
"""

from .archive import HDFArchive, HDFArchiveGroup, HDFArchiveInert
__all__ = ['HDFArchive', 'HDFArchiveGroup', 'HDFArchiveInert']
