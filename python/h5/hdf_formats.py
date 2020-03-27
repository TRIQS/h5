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

class HDF5Scheme:
    def __init__(self, classname, modulename, doc, hdf_format, read_fun) : 
        self.classname, self.modulename, self.doc, self.read_fun = classname, modulename,doc, read_fun
        self.format_name = hdf_format

    def __str__(self) : 
        return """
        Name of the class : %s
        Name of the module : %s
        Documentation : %s"""%(self.classname,self.modulename,self.doc)
        
_hdf5_formats_dict= {}

def register_class (cls, doc = None, read_fun = None, hdf5_format = None):
    """
     For each class, register it with::

         from HDFArchive_Schemes_dict import register_class 
         register_class (GfImFreq, doc= doc_if_different_from cls._hdf5_format_doc_ )

    """
    print("REGISTER", hdf5_format)
    hdf_format = hdf5_format or (cls._hdf5_format_ if hasattr(cls,"_hdf5_format_") else cls.__name__) 
    assert hdf_format not in _hdf5_formats_dict, "class %s is already registered"%hdf_format
    doc = doc if doc else (cls._hdf5_format_doc_ if hasattr(cls,"_hdf5_format_doc_") else {})
    _hdf5_formats_dict [hdf_format] = HDF5Scheme (cls.__name__, cls.__module__,doc, hdf_format, read_fun)


# Backward compatibility layer
_hdf5_formats_bck_compat = [] # List of (regex, format, lambda)

def register_backward_compatibility_method(regex, clsname, fun):  
    """
    regex : the regular expression to match the hdf_format (e.g. "Gf" for GfImfreq_x_whatever....)
    clsname : the class name that it corresponds to
    fun : a lambda taking hdf_format and returning a dict 
          field_name -> hdf_format
          to read old data where not every subobjects have a hdf_format.
    """
    _hdf5_formats_bck_compat.append((regex,_hdf5_formats_dict[clsname], fun))

# Functions used in the hdf_archive implementation

def hdf_format_access_for_write(hdf_format): 
    """Given a hdf_format, return the format. Must match exactly. Used in WRITING"""
    # If present exactly, we return it
    if hdf_format in _hdf5_formats_dict : 
        return _hdf5_formats_dict[hdf_format]
    raise KeyError("HDF5 Data Scheme %s is not registered"%hdf_format)

def hdf_format_access_for_read (hdf_format): 
    """
    Given a hdf_format, return the format, result of compat func or None. 
    Using compatibility layer. Used in READING
    """
    # If present exactly, we return it
    if hdf_format in _hdf5_formats_dict : 
        return _hdf5_formats_dict[hdf_format], None
    
    # Enter compatibility mode.
    l = [(r,s,l) for (r,s,l) in _hdf5_formats_bck_compat if re.match(r, hdf_format)]
    if len(l) ==0 : 
        raise KeyError("HDF5 Data Scheme %s is not registered and no backward compatibility found"%hdf_format)
    if len(l) >1 : 
        raise KeyError("HDF5 Data Scheme %s : ambiguous backward compatibility layers : %s"%([r for (r,s,ll) in l]))
    r,s,l = l[0]
    return s, l(hdf_format)


