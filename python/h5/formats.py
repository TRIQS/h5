# Copyright (c) 2019-2020 Simons Foundation
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

"""
Registration mechanism for archive-compatible Python classes.

To be written to and reconstructed from an HDF5 archive, a Python class is
associated with an HDF5 *format* string. This module maintains the registry that
maps format strings to the information needed to rebuild the corresponding
objects, together with the functions to populate and query it.
"""

import re
from copy import deepcopy

class FormatInfo:
    """
    Metadata describing how to reconstruct a Python class from an HDF5 group.

    Created and stored in a module-level registry by :func:`register_class`,
    and looked up at read time by :func:`get_format_info` from the
    ``Format`` attribute of the HDF5 group.

    Parameters
    ----------
    classname : str
        Name of the Python class to reconstruct.
    modulename : str
        Name of the module the class must be imported from.
    doc : str or dict
        Documentation associated with the format.
    hdf5_format : str
        The HDF5 format string this info is registered under. Stored on the
        instance as the ``format_name`` attribute.
    read_fun : callable or None
        Optional custom read function ``read_fun(group, key)``.

    Attributes
    ----------
    classname : str
        Name of the Python class to reconstruct.
    modulename : str
        Name of the module the class must be imported from.
    doc : str or dict
        Documentation associated with the format.
    read_fun : callable or None
        Optional custom read function ``read_fun(group, key)``.
    format_name : str
        The HDF5 format string (from the ``hdf5_format`` argument).
    backward_compat : dict
        Backward-compatibility mapping ``field_name -> hdf5_format``; empty for an
        exact match.
    """
    def __init__(self, classname, modulename, doc, hdf5_format, read_fun) :
        self.classname, self.modulename, self.doc, self.read_fun = classname, modulename, doc, read_fun
        self.format_name = hdf5_format
        self.backward_compat = {} # key ->  hdf5_format

    def __str__(self) :
        return """
        Name of the class : %s
        Name of the module : %s
        Documentation : %s"""%(self.classname,self.modulename,self.doc)

# Dictionary containing the FormatInfo for all hdf5_format strings
_formats_dict= {}

_formats_backward_compat = [] # List of (regex, format, lambda)

def register_class (cls, doc = None, read_fun = None, hdf5_format = None):
    """
    Register a Python class so it can be written to and read back from an archive.

    The class is stored in the module-level registry keyed by its HDF5 format
    string, which defaults to ``cls._hdf5_format_`` (if defined) or the class name.

    Parameters
    ----------
    cls : type
        The class to register.
    doc : str or dict, optional
        Documentation for the format. Defaults to ``cls._hdf5_format_doc_`` if
        defined, otherwise an empty dict.
    read_fun : callable, optional
        Custom read function ``read_fun(group, key)`` used to reconstruct the
        object instead of ``cls.__factory_from_dict__``.
    hdf5_format : str, optional
        Explicit format string to register under. Defaults to
        ``cls._hdf5_format_`` or ``cls.__name__``.

    Raises
    ------
    AssertionError
        If a class is already registered under the same format string.

    Examples
    --------
    >>> from h5.formats import register_class
    >>> register_class(GfImFreq, doc=doc_if_different_from_cls._hdf5_format_doc_)
    """
    hdf5_format = hdf5_format or (cls._hdf5_format_ if hasattr(cls,"_hdf5_format_") else cls.__name__)
    assert hdf5_format not in _formats_dict, "class %s is already registered"%hdf5_format
    doc = doc if doc else (cls._hdf5_format_doc_ if hasattr(cls,"_hdf5_format_doc_") else {})
    _formats_dict[hdf5_format] = FormatInfo(cls.__name__, cls.__module__, doc, hdf5_format, read_fun)


def register_backward_compatibility_method(regex, clsname, fun = lambda s: {}):
    """
    Register a backward-compatibility method for reading old data.

    Parameters
    ----------
    regex : str
        The regular expression to match the HDF5 format tag.
    clsname : str
        The class name that it corresponds to.
    fun : callable
        A lambda taking an HDF5 format tag and returning a dict that maps field
        names to HDF5 format tags.
    """
    _formats_backward_compat.append((regex, clsname, fun))


def get_format_info(hdf5_format):
    """
    Look up the :class:`FormatInfo` registered for ``hdf5_format``.

    If an exact match is found in the registry it is returned directly.
    Otherwise the backward-compatibility patterns registered via
    :func:`register_backward_compatibility_method` are tried in order; the
    first regex match yields a copy of the target class's :class:`FormatInfo`
    with its ``backward_compat`` field filled in.

    Parameters
    ----------
    hdf5_format : str
        The HDF5 format string to look up.

    Returns
    -------
    FormatInfo
        The format info for an exact match, or a copy augmented with
        backward-compatibility information for a regex match.

    Raises
    ------
    KeyError
        If no exact or compatible match is found, or if more than one
        compatibility pattern matches.
    """
    # If present exactly, we return it
    if hdf5_format in _formats_dict:
        return _formats_dict[hdf5_format]

    # Enter compatibility mode.
    match_lst = [(regex,clsname,fun) for (regex,clsname,fun) in _formats_backward_compat if re.match(regex,hdf5_format)]
    if len(match_lst) == 0:
        raise KeyError("H5 Format %s is not registered and no backward compatibility found"%hdf5_format)
    if len(match_lst) > 1:
        raise KeyError("H5 Format %s : ambiguous backward compatibility layers : %s"%([regex for (regex,clsname,fun) in match_lst]))
    regex,clsname,fun = match_lst[0]

    # Make a copy of the associated Format object and supplement it with backward compatibility information
    fmt = deepcopy(_formats_dict[clsname])
    fmt.backward_compat = fun(hdf5_format)

    return fmt
