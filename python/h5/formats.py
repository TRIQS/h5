################################################################################
#
# TRIQS: a Toolbox for Research in Interacting Quantum Systems
#
# Copyright (C) 2011-2017 by M. Ferrero, O. Parcollet
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
import re

class FormatInfo:
    """
    This class encapsulates essential information about for a particular h5 format.
    The information includes classname, modulename, documentaiton, the function to read.
    Further it provides information about the hdf5_format strings of subgroup keys,
    which is relevant for providing backward compatible reads.
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
     For each class, register it with::

         from h5.formats import register_class
         register_class (GfImFreq, doc= doc_if_different_from cls._hdf5_format_doc_ )

    """
    hdf5_format = hdf5_format or (cls._hdf5_format_ if hasattr(cls,"_hdf5_format_") else cls.__name__)
    assert hdf5_format not in _formats_dict, "class %s is already registered"%hdf5_format
    doc = doc if doc else (cls._hdf5_format_doc_ if hasattr(cls,"_hdf5_format_doc_") else {})
    _formats_dict[hdf5_format] = FormatInfo(cls.__name__, cls.__module__, doc, hdf5_format, read_fun)



def register_backward_compatibility_method(regex, clsname, fun = lambda s: {}):
    """
    regex : the regular expression to match the hdf5_format (e.g. "Gf" for GfImfreq_x_whatever....)
    clsname : the class name that it corresponds to
    fun : a lambda taking hdf5_format and returning a dict
          field_name -> hdf5_format
          to read old data where not every subobjects have a hdf5_format.
    """
    _formats_backward_compat.append((regex,_formats_dict[clsname], fun))

def get_format_info(hdf5_format):
    """
    Given an hdf5_format string, return the associated FormatInfo object.
    """
    # If present exactly, we return it
    if hdf5_format in _formats_dict :
        return _formats_dict[hdf5_format]

    # Enter compatibility mode.
    l = [(r,fmt,l) for (r,fmt,l) in _formats_backward_compat if re.match(r, hdf5_format)]
    if len(l) ==0 :
        raise KeyError("H5 Format %s is not registered and no backward compatibility found"%hdf5_format)
    if len(l) >1 :
        raise KeyError("H5 Format %s : ambiguous backward compatibility layers : %s"%([r for (r,s,ll) in l]))
    r,fmt,l = l[0]
    fmt.backward_compat = l(hdf5_format)

    return fmt
