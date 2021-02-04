# Copyright (c) 2019-2020 Simons Foundation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http:#www.apache.org/licenses/LICENSE-2.0.txt
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


import numpy,string
from .archive import *
from . import _h5py as h5
 
class HDFArchiveGroupBasicLayer:
    _class_version = 1

    def __init__(self, parent, subpath ):
        """  """
        self.options = parent.options
        self._group = parent._group.open_group(subpath) if subpath else parent._group
        self.ignored_keys = [] 
        self.cached_keys = list(self._group.keys())

    def _init_root(self, LocalFileName, open_flag, compress_level) :
        try :
            fich = h5.File(LocalFileName, open_flag)
        except :
            print("Cannot open the HDF file %s"%LocalFileName)
            raise
        # checking the version
        if open_flag not in ['r','r+','a'] :
            self._version = self._class_version
        else :
            try :
                self._version = int(fich.attrs['HDFArchive_Version'])
            except :
                self._version = 1
            if self._version > self._class_version :
                raise IOError("File %s is too recent for this version of HDFArchive module"%Filename)
        self._group = h5.Group(fich, compress_level)

    def is_group(self,p) :
        """Is p a subgroup ?"""
        assert len(p)>0 and p[0]!='/'
        return p in self.cached_keys and self._group.has_subgroup(p)

    def is_data(self,p) :
        """Is p a leaf ?"""
        assert len(p)>0 and p[0]!='/'
        return p in self.cached_keys and self._group.has_dataset(p)

    def write_attr (self, key, val) :
        self._group.write_attribute(key, val)

    def _read (self, key):
        return h5.h5_read(self._group, key)

    def _write(self, key, val) :
        h5.h5_write(self._group, key, val)

    def _flush(self):
        if bool(self._group): self._group.file.flush()

    def create_group (self,key):
        self._group.create_group(key)
        self.cached_keys.append(key)

    def keys(self) :
        return self.cached_keys

    def _clean_key(self,key, report_error=False) :
        if report_error and key not in self.cached_keys :
             raise KeyError("Key %s is not in archive !!"%key)
        if key in self.cached_keys :
          # FIXME
          # del self._group[key]
          self.cached_keys.remove(key)
        else: raise KeyError("Key %s is not in archive !!"%key)

