"""Write the Python-side DictNonStrKey reference archive read by the C++
h5_map test (MapWithPairKeyFromPython). Run via gen_dict_nonstrkey_refs.sh
with the h5 module on PYTHONPATH; the resulting dict_tuplekey.ref.h5 is
checked in. Not invoked by CTest."""

import numpy as np

from h5 import HDFArchive

with HDFArchive("dict_tuplekey.ref.h5", "w") as a:
    # tuple(str, str) -> int  (read into std::map<std::pair<std::string, std::string>, long>)
    a["map_int"] = {("a", "b"): 1, ("c", "d"): 2}
    # tuple(str, str) -> 1d double array  (read into std::map<..., std::vector<double>>)
    a["map_vec"] = {("a", "b"): np.array([1.0, 2.0]), ("c", "d"): np.array([3.0, 4.0])}
    # scalar int -> int  (read into std::map<long, long>)
    a["map_scalar"] = {1: 10, 2: 20}
